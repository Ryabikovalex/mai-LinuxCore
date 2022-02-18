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
    
    int files_count;
    struct c_file *files_list = getFilesListFromArchive(archive_fd, &files_count);
    
    if (files_list == NULL)
    {
        fprintf(stderr, "unpack: error for read of archive struct\n");
        exit_code = 1;
        goto unpack_free_memory_step_2;
    }
    
    for (int i=0; i<files_count; i++)
    {
        fprintf(stdout, "File: %s\nSize: %lu\n\n", files_list[i].name, files_list[i].size);
    }
    
    free(files_list);
  unpack_free_memory_step_2:
    close(archive_fd);
  unpack_free_memory_step_1:
    free(resolved_archive_path);
  unpack_free_memory_step_0:
    return exit_code;
}