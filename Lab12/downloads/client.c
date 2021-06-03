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

void main(int argc, char **argv)
{
    int socket_desc;

    struct sockaddr_in servaddr;

    if(argc != 2)
    {
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

    for(int i = 0; temp_ctr < strlen(argv[1]); i++)
    {
        temp_port[i] = argv[1][temp_ctr];
        temp_ctr++;
    }

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

    char FileName[255];
    char FileNameClient[255] = "downloads/";
    scanf("%s",FileName);
    send(socket_desc, FileName, 255, 0);

    strcat(FileNameClient,FileName);


    char text[1024];
    size_t datasize;

    int FileSize = read(socket_desc, &FileSize, 4);


    if(FileSize > 0)
    {
        FILE* fd = fopen(FileNameClient,"wb");

        for(int i = 0; i < FileSize+1; i++)
        {
            datasize = recv(socket_desc, text, 1024, 0);
            fwrite(text, sizeof(char), datasize, fd);
            text[0] = '\0';
        }
        fclose(fd);
        printf("File created: %s, %i kBytes recieved. \n", FileNameClient, FileSize);
    }
    else
    {
        printf("Seems like: %s does not exist. \n", FileName, datasize);
    }


}
