#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/errno.h>

int *allocateSharedMemory(size_t memSize, int &ShMemId)
{
    ShMemId = shmget(IPC_PRIVATE, memSize, 0600|IPC_CREAT|IPC_EXCL);

    if(ShMemId <= 0)
    {
		perror("Fatal error trying shmget()\n");
		return NULL;
    }

    int *memPtr = (int *)shmat(ShMemId, 0, 0);

    if(memPtr == NULL)
    {
        perror("Fatal error trying shmat()\n");
    }

    return memPtr;
}

void fillArrayRangedSem(int *ShMemPtr, int ArraySize, int Max, int Min, struct sembuf * Obj)
{
	for(int i = 0; i < ArraySize; i++)
	{
		*ShMemPtr = (rand() % (Max - Min + 1)) + Min;
		printf("%i is %i\n", i+1, *ShMemPtr);

        Obj [i].sem_op = 1;
        Obj [i].sem_flg = 0;
        Obj [i].sem_num = i;

        ShMemPtr++;
	};
	printf("Filled array with %i pseudo-rand int\n", ArraySize);
}

void tryGetSemaphore(int SemID, int SemNum)
{
    struct sembuf Lock;
	Lock.sem_op = -1;
	Lock.sem_flg = 0;
	Lock.sem_num = SemNum;
	semop(SemID, &Lock, 1);
}

int tryGetSemaphoreNW(int SemID,int SemNum)
{
    struct sembuf CheckSem;
    CheckSem.sem_op = 0;
	CheckSem.sem_flg = IPC_NOWAIT;
	CheckSem.sem_num = SemNum;
	return(semop(SemID, &CheckSem, 1));
}

void incSemaphore(int SemID, int SemNum)
{
    struct sembuf Init;
	Init.sem_op = 1;
	Init.sem_flg = 0;
	Init.sem_num = SemNum;
	semop(SemID, &Init, 1);
}

void sortArraySM(int *mem, int ArraySize, int SemID)
{
	int temp;

	for (int i = 0; i < ArraySize - 1; i++)
	{

        for (int j = 0; j < ArraySize - i - 1; j++)
        {

            if (mem[j] > mem[j + 1])
            {
                tryGetSemaphore(SemID, j);
                tryGetSemaphore(SemID, j+1);

                temp = mem[j];
                mem[j] = mem[j + 1];
                mem[j + 1] = temp;

                incSemaphore(SemID, j);
                incSemaphore(SemID, j+1);
            };

        };
    };
}

void printArray(int *ShMemPointer, int ArraySize)
{
            printf("Sorted Array\n");
		for(int i = 0; i < ArraySize; i++)
		{
			printf("%i is %i\n", i+1, *ShMemPointer);
			ShMemPointer++;
		};
}

int main (int argc, char *argv[])
{
	const size_t memSize = 1024; //alias of PAGE_SIZE

	const short unsigned int ArraySize = atoi(argv[1]);
	const short unsigned int ArrayMin = atoi(argv[2]);
	const short unsigned int ArrayMax = atoi(argv[3]);

	int ShMemId, SemID;

    char pathname[] = "main.cpp";
	key_t key; //IPC Key
    struct sembuf mySembuf[ArraySize];

    if((key = ftok(pathname,0)) < 0)
    {
        printf("Cant generate key");
        exit(-1);
    };
    if((SemID = semget(key,ArraySize, 0666 | IPC_CREAT)) < 0 )
    {
        printf("Cant get semid");
        exit(-1);
    }

	int *ShMemPointer = allocateSharedMemory(memSize, ShMemId);

	fillArrayRangedSem(ShMemPointer, ArraySize, ArrayMax, ArrayMin, mySembuf);
    if(semop(SemID, mySembuf, ArraySize) < 0)
    {
        printf("Can't initialize SemArray\n");
        printf ("err = %d\n", errno);
    }

    pid_t childId = fork();
    if(childId < 0)
	{
		perror("fork error");
	}
	else if(childId > 0)
	{
        int iCounter = 0;
        while (!waitpid(childId, NULL, WNOHANG))
        {
            printf("Itteration numer: %i \n", iCounter+1);
            for(int i = 0; i < ArraySize; i++)
            {

                if(tryGetSemaphoreNW(SemID,i) != 0)
                    {
                    //printf("%i is unlocked\n", i+1);
                    }
                    else
                    {
                    printf("%i is locked\n", i+1);
                    };

            }
            iCounter++;
        }
	}
	else
	{
		sortArraySM(ShMemPointer, ArraySize, SemID);
		exit(0);
	};

    printArray(ShMemPointer, ArraySize);
	//Freeing system resources
	if(shmctl(ShMemId,IPC_RMID, 0) == 0)
	{
		printf("Shared memory cleared!\n");
	}
	else
	{
		perror("Shared memory control problem!\n");
	};
	if(semctl(SemID,0,IPC_RMID,0) == 0)
	{
        printf("Semaphore array removed\n");
	}
	else
	{
    	printf("Semaphore error!");
	};


}
