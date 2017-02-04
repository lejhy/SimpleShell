#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 512

int main()
{
    char buffer[BUFF_SIZE];
    while(1){
        printf(">");
        gets(buffer);
    }
    return 0;
}
