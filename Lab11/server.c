#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
void sortArraySM(int *mem, int ArraySize)
{
	int temp;

	for (int i = 0; i < ArraySize - 1; i++)
	{
        for (int j = 0; j < ArraySize - i - 1; j++)
        {
            if (mem[j] < mem[j + 1])
            {
                temp = mem[j];
                mem[j] = mem[j + 1];
                mem[j + 1] = temp;
            };
        };
    };
}

void main(int argc, char *argv[])
{
    int ArraySize;

    int listenSock_desc, clientSock_desc;

    int clien_len;

    int recieved_n;

    struct sockaddr_in servaddr, cliaddr;
    if((listenSock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror(NULL);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family= AF_INET; servaddr.sin_port= htons(atoi(argv[1])); servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenSock_desc, (struct sockaddr *) &servaddr,sizeof(servaddr)) < 0)
    {
        servaddr.sin_port = 0;
        if(bind(listenSock_desc, (struct sockaddr *) &servaddr,sizeof(servaddr)) < 0)
        {
            perror(NULL);
            close(listenSock_desc);
            exit(1);
        }
    }

    if(listen(listenSock_desc, 5) < 0){
        perror(NULL);
        close(listenSock_desc);
        exit(1);
    }

    socklen_t lengar = sizeof(servaddr);
    getsockname(listenSock_desc, (struct sockaddr *)&servaddr, &lengar);
    printf("Server port: %d\n\n", ntohs(servaddr.sin_port));

    while(1)
    {
        clien_len = sizeof(cliaddr);

        if((clientSock_desc = accept(listenSock_desc,(struct sockaddr *) &cliaddr, &clien_len)) < 0)
        {
            perror(NULL);
            close(listenSock_desc);
            exit(1);
        }

        read(clientSock_desc, &ArraySize, 4);
        ArraySize = ntohl(ArraySize);
        int opArray[ArraySize];
        int temp;
        for(int i = 0; i<ArraySize; i++)
        {
            read(clientSock_desc, &temp, sizeof(int));
            temp = ntohl(temp);
            opArray[i]=temp;
        }
        sortArraySM(opArray,ArraySize);
        for(int i = 0; i < ArraySize; i++)
        {
            temp = htonl(opArray[i]);
            write(clientSock_desc, &temp,sizeof(int));
        }
        close(clientSock_desc);
    }
    close(listenSock_desc);
}
