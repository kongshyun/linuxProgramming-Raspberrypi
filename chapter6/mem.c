#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char* str=malloc(sizeof(char)*11);
    strcpy(str,"1234567890");

    printf("%s\n",str);
    free(str);
    return 0;

}
