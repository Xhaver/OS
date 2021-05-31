#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define LAST_MESSAGE 255

struct msgbuf {
        long msg_type;
        int msg_text;
    };

const size_t msg_length = sizeof(int);
const size_t inc_msglen = sizeof(int[4]);

void swap (int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

bool NextSet(int *a, int n)
{
  int j = n - 2;
  while (j != -1 && a[j] >= a[j + 1]) j--;
  if (j == -1)
    return false; // больше перестановок нет
  int k = n - 1;
  while (a[j] >= a[k]) k--;
  swap(&a[j],&a[k]);
  int l = j + 1, r = n - 1; // сортируем оставшуюся часть последовательности
  while (l<r)
  {
      swap(&a[l], &a[r]);
      l++; r--;
  }

  return true;
}

void sortArraySM(int *mem, int ArraySize)
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

void *secondThread(void *arg)
{
    int mid = (int) arg; //short-int-char size == size of ptr - ez cast

    struct msgbuf income_msg;
    int cArray[4];

    int arraySize = 4;

    for(int i = 0; i < 4; i++)
    {
        if((msgrcv(mid, (struct msgbuf*) &income_msg, msg_length, 0,0) < 0) && income_msg.msg_type != 1)
        {
            printf("Can't recieve anything or incorrect msg type\n");
            exit(-1);
        }
        else
        {
            printf("Recieved %i msg: %i\n", i+1, income_msg.msg_text);
            cArray[i] = income_msg.msg_text;
        };
    };

    sortArraySM(cArray, arraySize);
    for(int i = 0; i < arraySize-1; i++)
    {
        if(cArray[i] == cArray[i+1])
        {
        swap(&cArray[i],&cArray[arraySize-1]);
        arraySize--;
        }
    }

    sortArraySM(cArray,arraySize);
    income_msg.msg_text = arraySize;

    msgsnd(mid, (struct msgbuf *) &income_msg, msg_length, 0);

    if(arraySize > 1)
    {
        for(int i = 0; i < arraySize; i++)
        {
            income_msg.msg_text = cArray[i];
            msgsnd(mid, (struct msgbuf *) &income_msg, msg_length, 0);
        }
        while(NextSet(cArray, arraySize))
        {
            for(int i = 0; i < arraySize; i++)
            {
                income_msg.msg_text = cArray[i];
                msgsnd(mid, (struct msgbuf *) &income_msg, msg_length, 0);
            }
        }
    } else
    {
        income_msg.msg_text = cArray[0];
        msgsnd(mid, (struct msgbuf *) &income_msg, msg_length, 0);
    }

    income_msg.msg_type = LAST_MESSAGE;
    msgsnd(mid, (struct msgbuf *) &income_msg, msg_length, 0);

    return NULL;
}


int main()
{
    int msgQuery_ID;
    char pathname[] = "main.c";
    key_t key;

    //Creating MSGQ
    if((key =ftok(pathname, 0)) < 0)
    {
    printf("Cant generate key\n");
    exit(-1);
    }
    if((msgQuery_ID = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
    printf("Cant get msg query \n");
    exit(-1);
    }
    //Creating 2-st thread
    pthread_t secondThID;

    int result = pthread_create(&secondThID, NULL, secondThread, (void*) msgQuery_ID);
    if(result != 0){
    printf ("Error on thread create, return value = %d\n", result);
    exit(-1);
    }
    printf("2st Thread created, sending messges\n");
    //Compiling and sending msgs
    struct msgbuf msg;
    msg.msg_type = 1;

    for(int i = 0; i < 4; i++)
	{
		msg.msg_text = (rand() % (9 - 1 + 1)) + 1;

        printf("%i msg: %i\n", i, msg.msg_text);

        if (msgsnd(msgQuery_ID, (struct msgbuf *) &msg, msg_length, 0) < 0)
        {
            printf("Can\'t send message to queue\n");
            msgctl(msgQuery_ID, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
	};
    sleep(1);

    int swap_counter = 0;

    msgrcv(msgQuery_ID, (struct msgbuf*) &msg, msg_length, 0,0);

    int printLen = msg.msg_text;
    int temp[printLen];
    int ctr = 0;

	while((msgrcv(msgQuery_ID, (struct msgbuf*) &msg, msg_length, 0,0) > 0) && msg.msg_type != LAST_MESSAGE)
    {
        temp[ctr] = msg.msg_text;
        if(ctr == printLen-1)
        {
            for(int i = 0; i < printLen; i++)
            {
            printf("%i ", temp[i]);
            }
            ctr = 0;
            swap_counter++;
            printf("\n");
        }
        else
        {
        ctr++;
        swap_counter++;
        }
    }

    if(swap_counter == 0)
    {
    printf("Why the commutator is silent...\n");
    msgctl(msgQuery_ID, IPC_RMID,(struct msqid_ds *) NULL);
    exit(-2);
    }
    int reference = 1;
    for(int i = 1; i <= printLen; i++)
    {
    reference = reference*i;
    }
    printf("Swaps done (reference %i): %i\n", reference,swap_counter/3);

    pthread_join(secondThID, (void **)NULL);

    //pid_t childId = fork();

    msgctl(msgQuery_ID, IPC_RMID,(struct msqid_ds *) NULL);

    return 0;
}
