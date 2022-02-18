#include "main.h"

int unpack(char *archive_name, char *dir_name)
{
    int exit_code = 0;
    
    char *resolved_archive_path;
    
    resolved_archive_path = realpath(archive_name, NULL);
    if (resolved_archive_path == NULL)
    {
        perror("unpack: archive");
        exit_code = errno;
        goto unpack_free_memory_step_0;
    }
    
    char *resolved_dir_path;
    
    resolved_dir_path = realpath(dir_name, NULL);
    if (resolved_dir_path == NULL)
    {
        perror("unpack: directory");
        exit_code = errno;
        goto unpack_free_memory_step_1;
    }
    void *temp_ptr = realloc(resolved_dir_path, strlen(resolved_dir_path)+1 + 1);
    if (temp_ptr == NULL)
    {        
        perror("unpack");
        exit_code = errno;
        goto unpack_free_memory_step_2;
    }
    resolved_dir_path = (char *)temp_ptr;
    resolved_dir_path[strlen(resolved_dir_path)+2] = '\0';
    resolved_dir_path[strlen(resolved_dir_path)+1] = '/';
    
    int archive_fd = open(resolved_archive_path, O_RDONLY);
    
    if (archive_fd == -1)
    {
        perror("unpack");
        exit_code = errno;
        goto unpack_free_memory_step_2;
    }
    
    void *buf = malloc(BUFFER_SIZE);
    
    if (buf == NULL)
    {
        fprintf(stderr, "unpack: can't allocate buffer: %ud bytes\n", (unsigned int)BUFFER_SIZE);
        exit_code = errno;
        goto unpack_free_memory_step_3;
    }
    
    if (read(archive_fd, buf, 1) == -1)
    {
        perror("unpack");
        exit_code = errno;
        goto unpack_free_memory_step_4;
    }
    if (*buf != RECORD_SEPARATOR)
    {
        fprintf(stderr, "unpack: file is not archive\n");
        exit_code = 1;
        goto unpack_free_memory_step_4;
    }
    
    size_t files_count;
    struct c_file *files_list = getFilesListFromArchive(archive_fd, &files_count);
    
    if (files_list == NULL)
    {
        fprintf(stderr, "unpack: error for read of archive struct\n");
        exit_code = 1;
        goto unpack_free_memory_step_4;
    }
    
    for (int i=0; i<files_count; i++)
    {
        char *full_filename = (char *)malloc(strlen(resolved_dir_path)+strlen(files_list[i].name)+1);
        if (full_filename == NULL)
        {
            perror("unpack");
            exit_code = errno;
            goto unpack_free_memory_step_5;
        }
        
        strcpy(full_filename, resolved_dir_path);
        strcpy(full_filename+strlen(resolved_dir_path), files_list[i].name);
        
        int file_fd = open(full_filename, O_CREAT | O_EXCL | O_WRONLY);
        
        if (file_fd == -1)
        {
            if (errno == EEXIST)
            {
                fprintf(stdout, "unpack: %s is exsisted and will not be unpacked\n", full_filename);
                lseek(archive_fd, file->size + 1, SEEK_CUR);
            }
            else
            {
                free(full_filename);
                perror("unpack");
                exit_code = errno;
                goto unpack_free_memory_step_5;
            }
        }
        else
        {
            int count;
            if (read(archive_fd, buf, 1) == -1)
            {
                close(file_fd);
                free(full_filename);
                perror("unpack");
                exit_code = errno;
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
                    perror("unpack");
                    exit_code = errno;
                    goto unpack_free_memory_step_5;
                }
                count = write(file_fd, buf, count);
                if (count == -1)
                {
                    close(file_fd);
                    free(full_filename);
                    perror("unpack");
                    exit_code = errno;
                    goto unpack_free_memory_step_5;
                }
                read_c += count;
            }while(count != 0);
            close(file_fd);
        }
        
        free(full_filename);
    }
    
  unpack_free_memory_step_5:
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