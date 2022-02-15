#include <sys/stat.h>
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

unsigned long get_fsize(char *file_path)
{
    struct stat st;
    stat(file_path, &st);
    off_t size = st.st_size;

    return size;
}