#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char* str=malloc(sizeof(char)*5);
    strcpy(str,"1234567890");

    printf("%s\n",str);

    return 0;

}
