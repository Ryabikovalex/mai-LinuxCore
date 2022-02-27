#include <sys/stat.h>
#include "stdlib.h"
#include "stdio.h"
#include "main.h"

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

unsigned long get_file_size(char *file_path)
{
    struct stat st;
    stat(file_path, &st);
    off_t size = st.st_size;

    return size;
}

struct c_file * get_files_list_from_archive(int archive_fd, int *files_count)
{
    
    lseek(archive_fd, 0, SEEK_SET);
    
    long read_c = 0;
    int count;
    void *buf = malloc(BUFFER_SIZE);
    
    if (buf == NULL)
    {
        printf("error: Can't allocate buffer: %u bytes\n", (unsigned int)BUFFER_SIZE);
        return NULL;
    }
    
    count = read(archive_fd, buf, sizeof(char));
    if (count == -1)
    {
        printf("error: Can't read archive\n");
        free(buf);
        return NULL;
    }
    
    read_c += count;
    
    *files_count = 0;
    struct c_file *files_list = NULL;
    
    while (*((char *)buf) == RECORD_SEPARATOR)
    {
        safe_realloc((void **)&files_list, (*files_count+1)*sizeof(struct c_file));
        if (files_list == NULL)
        {
            printf("error: Not enough memory to read archive list\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        
        files_list[*files_count].name = (char *)malloc(STR_MAX_SIZE + 1);
        if (files_list[*files_count].name == NULL)
        {
            printf("error: Not enough memory to read archive list\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        files_list[*files_count].name[0] = '\0';
        
        u_int64_t size = 0;
        u_int64_t position = 0;
        size_t buff_pos = 0;
        
        // Read offset
        count = read(archive_fd, &(files_list[*files_count].size), sizeof(u_int64_t));
        if (count == -1)
        {
            printf("error: Can't read archive\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        if (count < sizeof(u_int64_t))
        {
            printf("error: Wrong file offset. Archive corrupted\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        read_c += count;
        
        // Read size
        count = read(archive_fd, &(files_list[*files_count].size), sizeof(u_int64_t));
        if (count == -1)
        {
            printf("error: Can't read archive\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        if (count < sizeof(u_int64_t))
        {
            printf("error: Wrong file size. Archive corrupted\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        read_c += count;
        
        // Read filename
        count = read(archive_fd, buf, BUFFER_SIZE);
        if (count == -1)
        {
            printf("error: Can't read archive\n");
            free(buf);
            free(files_list);
            return NULL;
        }
        read_c += count;
        
        char *filename = files_list[*files_count].name;
        size_t str_len = strlen(filename);
        
        void *temp_a = memchr(buf, RECORD_SEPARATOR, count);
        void *temp_b = memchr(buf, BINARY_SEPARATOR, count);
        void *temp = (temp_a == NULL) ? temp_b : temp_a;
        
        while (temp == NULL)
        {
            if (str_len + count > STR_MAX_SIZE)
            {
                printf("error: Can't read archive\n");
                free(buf);
                free(files_list);
                return NULL;
            }
            memcpy(filename + str_len, buf, count);
            filename[str_len+count] = '\0';
            str_len = strlen(filename);
            count = read(archive_fd, buf, BUFFER_SIZE);
            if (count == -1)
            {
                printf("error: Can't read archive\n");
                free(buf);
                free(files_list);
                return NULL;
            }
            read_c += count;
            temp_a = memchr(buf, RECORD_SEPARATOR, count);
            temp_b = memchr(buf, BINARY_SEPARATOR, count);
            temp = (temp_a == NULL) ? temp_b : temp_a;
        }
        memcpy(filename + str_len, buf, temp - buf);
        filename[str_len + (temp - buf)] = '\0';
        
        // Set to next record
        read_c -= count - (temp - buf);
        lseek(archive_fd, read_c, SEEK_SET);
        read_c += read(archive_fd, buf, sizeof(char));
        (*files_count)++;
    }
    lseek(archive_fd, -1, SEEK_CUR);
    
    free(buf);
    return files_list;
}

int accumulate_files_from_dir(const char *dir_path, char **acc_path, struct c_file ***accumulate,
                              u_int32_t *acc_size, int *depth)
{
    if (accumulate == NULL || acc_size == NULL || dir_path == NULL)
    {
        printf("error: Missing arguments\n");
        return 1;
    }
    if (depth == NULL || *depth == DIR_DEPTH)
    {
        return 0;
    }

    int exit_code = 0;

    // Store root dir for accumulate
    char *d_path        = calloc(STR_MAX_SIZE + 1, sizeof(char));
    // Store relative path to root dir
    char *relative_path = calloc(STR_MAX_SIZE + 1, sizeof(char));
    // Store resolved path to file / dir
    char *solved_d_path = calloc(STR_MAX_SIZE + 1, sizeof(char));
    if (d_path == NULL || relative_path == NULL || solved_d_path == NULL)
    {
        printf("error: Not enough memory. Requested %d bytes\n", STR_MAX_SIZE);
        exit_code = 1;
        goto accumulate_files_from_dir_exit;
    }

    // Init d_path strings
    memset(d_path, 0, STR_MAX_SIZE + 1);
    memset(solved_d_path, 0, STR_MAX_SIZE + 1);
    memset(relative_path, 0, STR_MAX_SIZE + 1);
    acc_path[0][strlen(acc_path[0])] = '/';
    acc_path[0][strlen(acc_path[0])] = 0;
    memcpy(relative_path, acc_path[0], strlen(acc_path[0]));
    memcpy(d_path, dir_path, strlen(dir_path));

    // Get full path
    strcat(d_path, relative_path);

    realpath(d_path, solved_d_path);
    DIR *dir_d = opendir(solved_d_path);

    if (dir_d == NULL)
    {
        printf("error: %s\n", strerror(errno));
        exit_code = 1;
        goto accumulate_files_from_dir_exit;
    } else
    {
        struct dirent *dir;
        while ((dir = readdir(dir_d)) != NULL)
        {
            if (dir->d_type == DT_DIR && (dir->d_name[0] != '.'))
            {
                strcat(acc_path[0], dir->d_name);
                depth[0]++;
                exit_code += accumulate_files_from_dir(dir_path, acc_path, accumulate, acc_size, depth);
            } else if (dir->d_type == DT_REG) // If regular file
            {
                size_t file_size = 0;
                char *file_path  = calloc(STR_MAX_SIZE+1, sizeof(char));
                char *resolved_file_path  = calloc(STR_MAX_SIZE+1, sizeof(char));

                memset(file_path, 0, STR_MAX_SIZE+1);
                strcpy(file_path, relative_path);
                strcat(file_path, dir->d_name);

                memset(resolved_file_path, 0, STR_MAX_SIZE+1);
                strcpy(resolved_file_path, solved_d_path);
                file_size = strlen(resolved_file_path);
                resolved_file_path[file_size] = '/';
                resolved_file_path[file_size+1] = 0;
                // Get size of file
                file_size = get_file_size(strcat(resolved_file_path, dir->d_name));
                // Add new item
                safe_realloc(((void **) accumulate), (acc_size[0] + 1)* sizeof(struct c_file *));
                accumulate[0][acc_size[0]] = calloc(sizeof(struct c_file), 1);
                accumulate[0][acc_size[0]]->name = calloc(sizeof(char), STR_MAX_SIZE + 1);
                strcpy(accumulate[0][acc_size[0]]->name, file_path);
                accumulate[0][acc_size[0]]->size = file_size;
                acc_size[0]++;

                free(file_path);
                free(resolved_file_path);
            } else if (dir->d_type == DT_LNK)
            {
                printf("warn: Skipped link: '%s'\n", dir->d_name);
            }

        }
        closedir(dir_d);
    }

    accumulate_files_from_dir_exit:
    free(d_path);
    free(relative_path);
    free(solved_d_path);
    return exit_code;
}