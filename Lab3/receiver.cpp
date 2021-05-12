#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

using namespace std;

int main(int argv, char *argc[])
{
	const int ArraySize = 20;

    if(argv <= 1)
    {
    	printf("Not enough input params");
    	return -1;
    };

    char *paramStr = argc[1];
    int memId = atoi(paramStr);

    if (memId == 0)
    {
        printf("incorrect parameter string: %s\n", paramStr);
        return -2;
    }

    printf("receiving the memory data: shmid = %i\n", memId);

    int *mem = (int *)shmat(memId, 0, 0);
	if (NULL == mem)
	{
		printf("error with shmat()\n");
		return -3;
	};
	printf("RECEIVED ARRAY:\n");
	for(int i = 0; i < ArraySize; i++)
	{
		printf("%i\n",mem[i]); //using compile conversion x[n] = *(x+n)
	};

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
 
    printf("SORTED ARRAY:\n");
    for(int i = 0; i < ArraySize; i++)
	{
		printf("%i\n",mem[i]); //using compile conversion x[n] = *(x+n)
	};
	
	char callbuf[1024];
	sprintf(callbuf, "ipcrm -m %i", memId);
	system(callbuf);
	
	return 0;
}

