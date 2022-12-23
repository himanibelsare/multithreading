#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

typedef long long ll;

struct timespec startTime; // time when prog starts running
struct timespec currTime; // current time
struct timespec patienceTime; // patience time for a student
int runTime; //seconds since program has been executed

//semaphores
sem_t studentSemaphore;

int notWashedCount = 0;
int totalTimeWasted = 0;

struct stu
{
    int arrivalTime;
    int washTime;
    int patienceTime;
    int index;
};

int compare(const void *s1, const void *s2)
{
    struct stu *e1 = (struct stu *)s1;
    struct stu *e2 = (struct stu *)s2;
    if(e1->arrivalTime != e2->arrivalTime)
        return e1->arrivalTime > e2->arrivalTime;
    else
        return e1->index < e2->index;
}

void getInput(struct stu* s, int a, int w, int t, int i)
{
    // printf("%d ", i);
    s->arrivalTime = a;
    s->washTime = w;
    s->patienceTime = t;
    s->index = i;
    // printf("%d %d %d\n", s->arrivalTime, s->washTime, s->patienceTime);
}

void* routine(void* parameters)
{
    int *args = (int*)parameters;

    clock_gettime(CLOCK_REALTIME, &currTime);
    // printf("sleep for: %ld\n", args[0] - (currTime.tv_sec - startTime.tv_sec));
    runTime = (currTime.tv_sec*1000000) - (startTime.tv_sec*1000000);
    usleep(args[0]*1000000 - runTime + args[3]);
    printf("%d: Student %d arrives\n", args[0], args[3] + 1);
    // printf("should be same as: %d\n", args[0]);

    clock_gettime(CLOCK_REALTIME, &patienceTime);
    patienceTime.tv_sec += args[2]; //arrive time + patience time
    // printf("patience until: %ld\n", patienceTime.tv_sec - startTime.tv_sec);

    if(sem_timedwait(&studentSemaphore, &patienceTime) != -1)
    {
        //student can wash
        clock_gettime(CLOCK_REALTIME, &currTime);  // time wasted = lock secured time - arrive time
        runTime = currTime.tv_sec - startTime.tv_sec;
        printf("\033[1;32m");
        printf("%d: Student %d starts washing\n", runTime, args[3]+1);
        printf("\033[0m");
        totalTimeWasted += (currTime.tv_sec - startTime.tv_sec - args[0]);

        //washing: upto m students can use the block
        usleep((args[1]*1000000)- 100000); //wash time
        
        //done washing
        clock_gettime(CLOCK_REALTIME, &currTime);
        runTime = currTime.tv_sec - startTime.tv_sec;
        printf("\033[01;33m");
        printf("%d: Student %d leaves after washing\n", runTime, args[3]+1);
        printf("\033[0m");

        //leaving the machine
        sem_post(&studentSemaphore);
    }
    else
    {
        //student cant wash
        clock_gettime(CLOCK_REALTIME, &currTime);
        runTime = currTime.tv_sec - startTime.tv_sec;
        totalTimeWasted += runTime - args[0];

        printf("\033[1;31m");
        printf("%d: Student %d leaves without washing\n", runTime, args[3]+1);
        printf("\033[0m");
        notWashedCount++;
    }
}

int main()
{
    int n, m;
    scanf("%d %d", &n, &m);
    int a, w, t;
    struct stu students[n];

    for (int i = 0; i < n; i++)
    {   
        scanf("%d %d %d", &a, &w, &t);
        getInput(&students[i], a, w, t, i);
    }
    clock_gettime(CLOCK_REALTIME, &startTime);

    sem_init(&studentSemaphore, 0, m);
    pthread_t studentThreads[n];

    qsort(students, n, sizeof(struct stu), compare);
    int ret;
    for(int i = 0; i < n; i++)
    {
        //pthread create
        clock_gettime(CLOCK_REALTIME, &currTime);
        runTime = currTime.tv_sec - startTime.tv_sec;
        // printf("rn2: %d\n", runTime);
       
        //allocate arguments for routine
        int *arguments = malloc(6*sizeof(int));
        arguments[0] = students[i].arrivalTime;
        arguments[1] = students[i].washTime;
        arguments[2] = students[i].patienceTime;
        arguments[3] = students[i].index;
        ret = pthread_create(&studentThreads[i], NULL, &routine, arguments);
        if(ret < 0)
            perror("Failed to create thread");
           
    }
    for (int i = 0; i < n; i++)
    {
        //pthread join
        pthread_join(studentThreads[i], NULL);
    }

    //destroy semaphores
    sem_destroy(&studentSemaphore);

    //stats
    printf("%d\n%d\n", notWashedCount, totalTimeWasted);
    if(notWashedCount/(double)n >= 0.25)
        printf("Yes\n");
    else
        printf("No\n");

    return 0;
}