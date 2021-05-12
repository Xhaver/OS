#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *firstThread(void *dummy)
{
    for(int i = 1; i <= 10; i++)
    {
    printf("Hello Threads (%i)\n", i);
    sleep(1);
    };

    return NULL;
}

void *secondThread(void *dummy)
{
    for(int i = 1; i <= 12; i++)
    {
    printf("This is iterration %i\n", i);
    sleep(2);
    };

    return NULL;
}

int main()
{
    pthread_t firstThid, secondThid;
    
    int result;
    
    result = pthread_create(&firstThid,NULL, firstThread, NULL);
    if(result != 0){
    printf ("Error on thread create, return value = %d\n", result);
    exit(-1);
    }
    printf("1st Thread created, thid = %d\n", firstThid);
    result = pthread_create(&secondThid,NULL, secondThread, NULL);
    if(result != 0){
    printf ("Error on thread create, return value = %d\n", result);
    exit(-1);
    }
    printf("2nd Thread created, thid = %d\n", secondThid);
    
    pthread_join(firstThid, (void **)NULL);
    pthread_join(secondThid, (void **)NULL);
    
    return 0;
}
