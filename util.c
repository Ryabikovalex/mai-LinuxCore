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

unsigned long get_fsize(char *file_path)
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