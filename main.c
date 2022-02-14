#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
// POSIX
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
// Pthread
#include <pthread.h>

//User defined
#include "util.h"

// Function for creating archive
int pack(char *dir_name);
unsigned long get_fsize(char *file_path);

struct c_file {
    char *name;
    unsigned long size;
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("error: Missing arguments");
        return -1;
    } else {
        if (strcmp(argv[1],"pack") == 0 )
        {
            pack(argv[2]);
        }
        return 0;
    }
}

int pack(char *dir_path){
    DIR *d;
    char *resolved_path = calloc(sizeof (char), 255);
    struct dirent *dir;

    realpath(dir_path, resolved_path);
    d = opendir(resolved_path);

    // Added / for use in loop
    resolved_path[strlen(resolved_path)] = '/';
    resolved_path[strlen(resolved_path)] = '\0';

    if (d == NULL)
    {
        printf("error: %s\n", strerror(errno));
        free(resolved_path);
        return 1;
    }
    else
    {
        unsigned list_size = 0;
        struct c_file** file_list = calloc(sizeof(struct c_file*), list_size);

        while ((dir = readdir(d)) != NULL) {
            if(dir->d_type == DT_DIR && (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0))
            {
                printf("warn: Skipped directory: %s\n", dir->d_name);
            } else if (dir->d_type == DT_REG) // If regular file
            {
                // Get size of file
                unsigned long size = get_fsize(strcat(resolved_path, dir->d_name));
                // Add new item
                safe_realloc(((void**)&file_list), list_size+1);
                file_list[list_size] = calloc(sizeof(struct c_file), 1);
                file_list[list_size]->name = calloc(sizeof(char), 255);
                strcpy(file_list[list_size]->name, dir->d_name);
                file_list[list_size]->size = size;

            } else if (dir->d_type == DT_LNK){
                printf("warn: Skipped link: '%s'\n", dir->d_name);
            }
        }
        closedir(d);

        // Memory free
        for (int i=0; i < list_size; i++)
        {
            free(file_list[i]->name);
            free(file_list[i]);
        }
        free(file_list);
        free(resolved_path);
        return 0;
    }
}

unsigned long get_fsize(char *file_path)
{
    struct stat st;
    stat(file_path, &st);
    off_t size = st.st_size;

    return size;
}