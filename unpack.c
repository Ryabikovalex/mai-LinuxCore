#include "main.h"

int unpack(char *archive_name, char *dir_name)
{
    int exit_code = 0;
    
    char *resolved_archive_path;
    
    resolved_archive_path = realpath(archive_name, NULL);
    if (resolved_archive_path == NULL)
    {
        printf("error: %s\n", strerror(errno));
        goto unpack_free_memory_step_0;
    }
    
    char *resolved_dir_path;
    
    resolved_dir_path = realpath(dir_name, NULL);
    if (resolved_dir_path == NULL)
    {
        printf("error: %s\n", strerror(errno));
        exit_code = 1;
        goto unpack_free_memory_step_1;
    }
    void *temp_ptr = realloc(resolved_dir_path, strlen(resolved_dir_path)+1 + 1);
    if (temp_ptr == NULL)
    {
        printf("error: Not enough memory\n");
        exit_code = 1;
        goto unpack_free_memory_step_2;
    }
    resolved_dir_path = (char *)temp_ptr;
    resolved_dir_path[strlen(resolved_dir_path)+1] = '\0';
    resolved_dir_path[strlen(resolved_dir_path)] = '/';
    
    int archive_fd = open(resolved_archive_path, O_RDONLY);
    
    if (archive_fd == -1)
    {
        printf("error: Can't open file \"%s\"\n", resolved_archive_path);
        exit_code = 1;
        goto unpack_free_memory_step_2;
    }
    
    void *buf = malloc(BUFFER_SIZE);
    
    if (buf == NULL)
    {
        printf("error: Can't allocate buffer: %u bytes\n", (unsigned int)BUFFER_SIZE);
        exit_code = 1;
        goto unpack_free_memory_step_3;
    }
    
    if (read(archive_fd, buf, 1) == -1)
    {
        printf("error: Can't read form archive");
        exit_code = 2;
        goto unpack_free_memory_step_4;
    }
    if (*((char *)buf) != RECORD_SEPARATOR)
    {
        printf("error: File is not archive\n");
        exit_code = 1;
        goto unpack_free_memory_step_4;
    }
    
    int files_count;
    struct c_file *files_list = get_files_list_from_archive(archive_fd, &files_count);
    
    if (files_list == NULL)
    {
        printf("error: error for read of archive struct\n");
        exit_code = 1;
        goto unpack_free_memory_step_4;
    }
    
    for (int i=0; i<files_count; i++)
    {
        char *full_filename = (char *)malloc(strlen(resolved_dir_path)+strlen(files_list[i].name)+1);
        if (full_filename == NULL)
        {
            printf("error: Can't allocate buffer: %u bytes\n", (unsigned int)STR_MAX_SIZE);
            exit_code = 2;
            goto unpack_free_memory_step_5;
        }
        
        strcpy(full_filename, resolved_dir_path);
        strcpy(full_filename+strlen(resolved_dir_path), files_list[i].name);
        
        int file_fd = open(full_filename, O_TRUNC | O_EXCL | O_WRONLY);
        
        if (file_fd == -1)
        {
            free(full_filename);
            printf("error: %s\n", strerror(errno));
            exit_code = 1;
            goto unpack_free_memory_step_5;
        }
        else
        {
            int count;
            if (read(archive_fd, buf, 1) == -1)
            {
                close(file_fd);
                free(full_filename);
                printf("error: Can't read from archive\n");
                exit_code = 1;
                goto unpack_free_memory_step_5;
            }
            int read_c = 0;
            do
            {
                count = read(archive_fd, buf, (read_c + BUFFER_SIZE <= files_list[i].size) ? BUFFER_SIZE : files_list[i].size - read_c);
                if (count == -1)
                {
                    close(file_fd);
                    free(full_filename);
                    printf("error: Can't read from archive\n");
                    exit_code = 1;
                    goto unpack_free_memory_step_5;
                }
                count = write(file_fd, buf, count);
                if (count == -1)
                {
                    close(file_fd);
                    free(full_filename);
                    printf("error: Can't read from archive\n");
                    exit_code = 1;
                    goto unpack_free_memory_step_5;
                }
                read_c += count;
            }while(count != 0);
            close(file_fd);
        }
        
        free(full_filename);
    }
    
  unpack_free_memory_step_5:
    for(int i=0; i<files_count; i++){
        free(files_list[i].name);
    }
    free(files_list);
  unpack_free_memory_step_4:
    free(buf);
  unpack_free_memory_step_3:
    close(archive_fd);
  unpack_free_memory_step_2:
    free(resolved_dir_path);
  unpack_free_memory_step_1:
    free(resolved_archive_path);
  unpack_free_memory_step_0:
    return exit_code;
}