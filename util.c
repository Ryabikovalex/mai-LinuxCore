#include "stdlib.h"
#include "stdio.h"

void safe_realloc(void **ptr, unsigned int size)
{
    void *temp = realloc(*ptr, size);
    if (temp != NULL)
    {
        *ptr = temp;
    } else {
        printf("error: Memory leak\n");
    }
}