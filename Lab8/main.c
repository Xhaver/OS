#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc < 3) exit(-1);

    const char *inp_file = argv[1];
    const char *out_file = argv[2];

    int fileDesc = open(out_file, O_RDWR | O_CREAT, 0600);

    if (fileDesc < 0)
    {
        perror("error with open of out file");
        return -1;
    }

    struct stat inFileStat;
    stat(inp_file, &inFileStat);
    size_t input_size = inFileStat.st_size;

    ftruncate(fileDesc, input_size);

    char *mapPtr = mmap(0,input_size,PROT_WRITE,MAP_SHARED,fileDesc,0);

    close(fileDesc);

    fileDesc = open(inp_file, O_RDONLY);
    read(fileDesc, mapPtr, input_size);
    close(fileDesc);

    if(munmap((void *)mapPtr, input_size) != 0)
    {
        perror("error with writing in out file");
        return -1;
    }
}
