#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>




void clientService(int clientSock_desc,int clientNum)
{
    char FileName[255]; //memset(FileName,0,255);
    read(clientSock_desc, FileName, 255);
    FILE *fd;

    printf("Client %i requsted %s\n", clientNum, FileName);
    if((fd = fopen(FileName, "rb")) == NULL)
    {
        perror(NULL);
        fclose(fd);
        return;
    }

    fseek(fd, 0, SEEK_END);
    int size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    write(clientSock_desc, &size,sizeof(int));

    char buffer[1024]; //memset(buffer,0,1024);
    size_t rret, wret;
    int bytes_read;
    while (!feof(fd))
    {
        if ((bytes_read = fread(buffer, sizeof(char), 1024, fd)) > 0)
        {
            send(clientSock_desc, buffer, bytes_read, 0);
        }
        else
            break;
    }
    fclose(fd);
    return;
}
void main(int argc, char *argv[])
{

    int listenSock_desc, clientSock_desc;

    int clien_len;

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

    if(listen(listenSock_desc, 25) < 0){
        perror(NULL);
        close(listenSock_desc);
        exit(1);
    }

    socklen_t lengar = sizeof(servaddr);
    getsockname(listenSock_desc, (struct sockaddr *)&servaddr, &lengar);
    printf("Server port: %d\n", ntohs(servaddr.sin_port));

    unsigned clientsCounter = 0;
    pid_t pid;

    pid = fork();
        if(pid < 0)
        {
            perror(NULL);
            close(listenSock_desc);
            exit(1);
        }
        else if(pid == 0)
        {
            while(1)
            {
                    if((clientSock_desc = accept(listenSock_desc,(struct sockaddr *) &cliaddr, &clien_len)) < 0)
                    {
                        perror(NULL);
                        close(listenSock_desc);
                        exit(1);
                    }

                    pid = fork();
                    if(pid < 0)
                    {
                        perror(NULL);
                        close(listenSock_desc);
                        exit(1);
                    }
                    else if(pid == 0)
                    {
                        printf("Servicing client %i\n", ++clientsCounter);
                        close(listenSock_desc);
                        clientService(clientSock_desc, clientsCounter);
                        printf("Client %i diconnected\n", clientsCounter);
                        close(clientSock_desc);
                        exit(0);
                    }
                    else
                    {
                        ++clientsCounter;
                        close(clientSock_desc);
                    }
                }
                close(listenSock_desc);
            }
        else
        {
            char commandsBuffer[4] = "";

            while(1)
            {
                scanf("%s",commandsBuffer);
                if(strcmp(commandsBuffer,"exit") == 0)
                {
                    close(listenSock_desc);
                    kill(pid, SIGTERM);
                    return;
                }
                else
                {
                    printf("%s\n",commandsBuffer);
                }
                commandsBuffer[0] = '\0';
            }
        }
    return;
}

