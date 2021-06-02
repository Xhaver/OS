#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void fillArrayRangedSem(int *ShMemPtr, int ArraySize, int Max, int Min)
{
	for(int i = 0; i < ArraySize; i++)
	{
		*ShMemPtr = (rand() % (Max - Min + 1)) + Min;
		printf("%i is %i\n", i+1, *ShMemPtr);
        ShMemPtr++;
	};
	printf("Filled array with %i pseudo-rand int\n", ArraySize);
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


void main(int argc, char **argv)
{

    int ArraySize, ArrayMax, ArrayMin;
    scanf("%d%d%d", &ArraySize, &ArrayMin, &ArrayMax);

    int opArray[ArraySize];
    fillArrayRangedSem(opArray, ArraySize, ArrayMax, ArrayMin);

    int socket_desc;


    struct sockaddr_in servaddr;

    if(argc != 2){
        printf("Usage: a.out <IP address>:<port>\n");
        exit(1);
    }


    char temp_port[5];
    int temp_ctr=1;
    for(; argv[1][temp_ctr-1] != ':';)
    {
    temp_ctr++;
    }
    char ip_adr[temp_ctr];
    memset(ip_adr,0,sizeof ip_adr);
    strncpy(ip_adr,argv[1],temp_ctr-1);
    printf("%s\n",ip_adr);
    for(int i = 0; temp_ctr < strlen(argv[1]); i++)
    {
        temp_port[i] = argv[1][temp_ctr];
        temp_ctr++;
    }
    printf("%s\n",temp_port);
    int port;
    port = atoi(temp_port);

    if((socket_desc = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; servaddr.sin_port = htons(port);

    if(inet_aton(ip_adr, &servaddr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        close(socket_desc);
        exit(1);
    }

    if(connect(socket_desc, (struct sockaddr *) &servaddr,
    sizeof(servaddr)) < 0){
        perror(NULL);
        close(socket_desc);
        exit(1);
    }

    int intBuf = htonl(ArraySize);
    write(socket_desc,&intBuf,sizeof(int));

    for(int i = 0; i < ArraySize; i++)
    {
        intBuf = htonl(opArray[i]);
        write(socket_desc,&intBuf,sizeof(int));
    };

    for(int i = 0; i < ArraySize; i++)
    {
    read(socket_desc,&intBuf,sizeof(int));
    opArray[i] = ntohl(intBuf);
    }
    printArray(opArray,ArraySize);
    close(socket_desc);
}
