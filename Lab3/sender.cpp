#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

using namespace std;

int main()
{  
	const int ArraySize = 20;
	
    const size_t memSize = 1024; //alias of PAGE_SIZE

    int memId = shmget(IPC_PRIVATE, memSize, 0600|IPC_CREAT|IPC_EXCL);

    printf("shmid= %i\n", memId);

    if(memId <= 0)
    {
    printf("Fatal error trying shmget()\n");
    return -1;
    };

    int *mem = (int *)shmat(memId, 0, 0);

    if(mem == NULL)
    {
        printf("Fatal error trying shmat()\n");
        return -2;
    };  
    
    for(int i = 0; i < ArraySize; i++)
    {
    	*mem = rand();
    	mem++;
    }

    char callbuf[1024];
	sprintf(callbuf, "./receiver %i", memId);
	system(callbuf);

	return 0;
}
