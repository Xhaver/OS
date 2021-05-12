#include <sys/shm.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h>

//Allocating 1024 (4096 on fact) bytes for our tasks and return pointer to begging of it + changing outer value of memId
int *allocateSharedMemory(size_t memSize, int &memId) 
{
    memId = shmget(IPC_PRIVATE, memSize, 0600|IPC_CREAT|IPC_EXCL);
    
    if(memId <= 0)
    {
		perror("Fatal error trying shmget()\n");
		return NULL;
    }

    int *memPtr = (int *)shmat(memId, 0, 0);
    
    if(memPtr == NULL)
    {
        perror("Fatal error trying shmat()\n");
    }
    
    return memPtr;
}
//Fills array with (n = ArraySize) pseudo-random numbers;
void fillArray(int *MemPtr, int ArraySize)
{
	for(int i = 0; i < ArraySize; i++)
    {
    	*MemPtr = rand();
    	printf("%i is %i\n", i+1, *MemPtr);
    	MemPtr++;
    }
}

void sortArray(int *mem, int ArraySize)
{
	int temp;
	for (int i = 0; i < ArraySize - 1; i++) 
	{
        for (int j = 0; j < ArraySize - i - 1; j++) 
        {
            if (mem[j] > mem[j + 1]) 
            {
                temp = mem[j];
                mem[j] = mem[j + 1];
                mem[j + 1] = temp;
            };
        };
    };
}

int main()
{
	const int ArraySize = 20;
    const size_t memSize = 1024; //alias of PAGE_SIZE
    int memId;
    
    int *sharedMemPtr = allocateSharedMemory(memSize, memId);
    printf("ID of shared memory = %d\n", memId);
    
    printf("Unsorted Array:\n");
    fillArray(sharedMemPtr, ArraySize);    

	pid_t childId = fork();
	if(childId < 0)
	{
		perror("fork error");
	} 
	else if(childId > 0)
	{
		printf("Parent: This is main process. Child id = %d\n", childId);         
		printf("Parent: awaiting for child.\n");

		waitpid(childId, NULL, 0); 
		
		printf("Parent: execution is continued.\n"); 
	} 
	else
	{
		printf("Child: This is child process. My parrent id = %d\n",getppid());
		sortArray(sharedMemPtr, ArraySize);	
		
		int *MemPtr = sharedMemPtr;
		printf("Sorted Array\n");
		for(int i = 0; i < ArraySize; i++)
		{
			printf("%i is %i\n", i+1, *MemPtr);
			MemPtr++;
		};
		exit(0);
	};
	
	if(shmctl(memId,IPC_RMID, 0) == 0)
	{
		printf("Shared memory cleared!\n");	
	} 
	else 
	{
		perror("Shared memory control problem!");
	};
	
	
	return 0;
}
