#include "main.h"

int list(char *archive_name)
{
    int exit_code = 0;
    char *resolved_archive_path;
    
    resolved_archive_path = realpath(archive_name, NULL);
    if (resolved_archive_path == NULL)
    {
        perror("list");
        exit_code = errno;
        goto unpack_free_memory_step_0;
    }
    
    int archive_fd = open(resolved_archive_path, O_RDONLY);
    
    if (archive_fd == -1)
    {
        perror("unpack");
        exit_code = errno;
        goto unpack_free_memory_step_1;
    }
    
    struct List *files_list = getFilesListFromArchive(archive_fd);
    
    if (files_list == NULL)
    {
        fprintf(stderr, "unpack: error for read of archive struct\n");
        exit_code = 1;
        goto unpack_free_memory_step_2;
    }
    
    struct List *files_list_elem = files_list;
    
    while (files_list_elem != NULL)
    {
        struct c_file *file = (struct c_file *)files_list_elem->data;
        
        fprintf(stdout, "File: %s\nSize: %ul\n\n", file->name, file->size);
        
        files_list_elem = files_list_elem->next;
    }
    
    removeList(&files_list, freeFile);
  unpack_free_memory_step_2:
    close(archive_fd);
  unpack_free_memory_step_1:
    free(resolved_archive_path);
  unpack_free_memory_step_0:
    return exit_code;
}