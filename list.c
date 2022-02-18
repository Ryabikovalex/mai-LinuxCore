#include "main.h"

struct List * list(int archive_fd)
{
    
    lseek(archive_fd, 0, SEEK_SET);
    
    long read_c = 0;
    int count;
    void *buf = malloc(BUFFER_SIZE);
    
    if (buf == NULL)
    {
        fprintf(stderr, "list: can't allocate buffer: %ud bytes\n", BUFFER_SIZE);
        return NULL;
    }
    
    count = read(archive_fd, buf, sizeof(char));
    if (count == -1)
    {
        perror("list");
        free(buf);
        return NULL;
    }
    
    read_c += count;
    
    struct List *files_list = NULL;
    
    while (*buf == RECORD_SEPARATOR)
    {
        struct List *elem = insertElem(&files_list, -1, NULL, 0);
        if (elem == NULL)
        {
            fprintf(stderr, "list: can't create new element in list of files\n");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        
        elem->data = malloc(sizeof(struct File));
        if (elem->data == NULL)
        {
            fprintf(stderr, "list: can't create new element in list of files\n");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        
        ((struct File *)(elem->data))->name = (char *)malloc(STR_MAX_SIZE + 1);
        if (((struct File *)(elem->data))->name == NULL)
        {
            fprintf(stderr, "list: can't create new element in list of files\n");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        ((struct File *)(elem->data))->name[0] = '\0';
        
        u_int64_t size = 0;
        u_int64_t position = 0;
        size_t buff_pos = 0;
        
        // Read offset
        count = read(archive_fd, &(((struct File *)(elem->data))->size), sizeof(u_int64_t));
        if (count == -1)
        {
            perror("list");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        if (count < sizeof(u_int64_t))
        {
            fprintf(stderr, "list: archive corrupted\n");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        read_c += count;
        
        // Read size
        count = read(archive_fd, &(((struct File *)(elem->data))->size), sizeof(u_int64_t));
        if (count == -1)
        {
            perror("list");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        if (count < sizeof(u_int64_t))
        {
            fprintf(stderr, "list: archive corrupted\n");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        read_c += count;
        
        // Read filename
        count = read(archive_fd, buffer, BUFFER_SIZE);
        if (count == -1)
        {
            perror("list");
            free(buf);
            removeList(&files_list, freeFile);
            return NULL;
        }
        read_c += count;
        
        char *filename = ((struct File *)(elem->data))->name;
        size_t str_len = strlen(filename);
        
        void *temp_a = memchr(buffer, RECORD_SEPARATOR, count);
        void *temp_b = memchr(buffer, BINARY_SEPARATOR, count);
        void *temp = (temp_a == NULL) ? temp_b : temp_a;
        
        while (temp == NULL)
        {
            if (str_len + count > STR_MAX_SIZE)
            {
                fprintf(stderr, "list: archive corrupted\n");
                free(buf);
                removeList(&files_list, freeFile);
                return NULL;
            }
            memcpy(filename + str_len, buffer, count);
            filename[str_len+count] = '\0';
            str_len = strlen(filename);
            count = read(archive_fd, buffer, BUFFER_SIZE);
            if (count == -1)
            {
                perror("list");
                free(buf);
                removeList(&files_list, freeFile);
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
        read_c += read(file_d, buf, sizeof(char));
    }
    
    free(buf);
    return files_list;
}