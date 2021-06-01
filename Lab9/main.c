#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc < 3) exit(-1);

    const char *filePath = argv[1];
    const unsigned int bytesToRead   = atoi(argv[2]);

    FILE *fin = fopen(filePath, "r");
    if (NULL == fin)
    {
        perror("cannot open file");
        return -1;
    }

    char data[bytesToRead];
    int res = fread(data,sizeof(char), bytesToRead, fin);
    printf("fread readed: %i\n",res);
    if(res > 0)
    {
        printf("fread as CHAR: %s\n", data);
        printf("fread to ASCII conversion:\n");
        for(int i = 0; i < res; i++)
        {
            printf("%i ", (int)data[i]);
        }
    }


    fclose(fin);
    return 0;
}
