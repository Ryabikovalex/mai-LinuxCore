#include "main.h"

int pack(char *dir_path, char *archive_name)
{
    size_t size_t;
    char temp_c;
    DIR *d;
    char *resolved_dir_path = calloc(sizeof(char), STR_MAX_SIZE + 1);

    realpath(dir_path, resolved_dir_path);
    d = opendir(resolved_dir_path);

    if (d == NULL)
    {
        printf("error: %s\n", strerror(errno));
        free(resolved_dir_path);
        return 1;
    } else
    {
        int exit_code = 0;
        struct dirent *dir;
        unsigned list_size = 0;
        char *resolved_file_path = calloc(sizeof(char), STR_MAX_SIZE + 1);
        struct c_file **file_list = calloc(sizeof(struct c_file *), list_size);

        // Added / for use in loop
        size_t = strlen(resolved_dir_path);
        resolved_dir_path[size_t] = '/';
        resolved_dir_path[size_t + 1] = '\0';

        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_DIR && (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0))
            {
                printf("warn: Skipped directory: %s\n", dir->d_name);
            } else if (dir->d_type == DT_REG) // If regular file
            {
                memset(resolved_file_path, 0, STR_MAX_SIZE + 1);
                strcpy(resolved_file_path, resolved_dir_path);
                // Get size of file
                size_t = get_fsize(strcat(resolved_file_path, dir->d_name));
                // Add new item
                safe_realloc(((void **) &file_list), (list_size + 1)* sizeof(struct c_file *));
                file_list[list_size] = calloc(sizeof(struct c_file), 1);
                file_list[list_size]->name = calloc(sizeof(char), STR_MAX_SIZE + 1);
                strcpy(file_list[list_size]->name, dir->d_name);
                file_list[list_size]->size = size_t;
                list_size++;
            } else if (dir->d_type == DT_LNK)
            {
                printf("warn: Skipped link: '%s'\n", dir->d_name);
            }
        }
        // Opened archive
        char *resolved_archive_path = calloc(sizeof(char), STR_MAX_SIZE + 1);
        realpath(archive_name, resolved_archive_path);
        int archive_d = open(resolved_archive_path, O_CREAT | O_TRUNC | O_RDWR);

        u_int64_t accumulate = 0; // Binary start position

        // Accumulate
        for (int i = 0; i < list_size; i++)
        {
            // +2 char of Record and File separators
            accumulate += 1 + strlen(file_list[i]->name) + 2 * sizeof(u_int64_t);
        }
        temp_c = RECORD_SEPARATOR;
        for (int i = 0; i < list_size; i++)
        {
            accumulate += 1 + file_list[i]->size;
            write(archive_d, &temp_c, 1);
            write(archive_d, (void *) &accumulate, sizeof(u_int64_t));
            write(archive_d, (void *) &(file_list[i]->size), sizeof(u_int64_t));
            write(archive_d, file_list[i]->name, strlen(file_list[i]->name));
        }

        // Write files binary
        void *buffer = calloc(sizeof(void *), BUFFER_SIZE);
        u_int64_t write_c;
        u_int64_t read_c;
        temp_c = BINARY_SEPARATOR;
        if (buffer != NULL)
        {
            for (int i = 0; i < list_size; i++)
            {
                memset(resolved_file_path, 0, STR_MAX_SIZE + 1);
                strcpy(resolved_file_path, resolved_dir_path);
                strcat(resolved_file_path, file_list[i]->name);
                int file_d = open(resolved_file_path, O_RDONLY);
                if (file_d < 0)
                {
                    printf("error: %s\n", strerror(errno));
                    exit_code = 1;
                } else
                {
                    if (write(archive_d, &temp_c, 1))
                    {
                        size_t = file_list[i]->size;
                        while (size_t > 0)
                        {
                            read_c = read(file_d, buffer, BUFFER_SIZE);
                            write_c = write(archive_d, buffer, read_c);
                            if (read_c != write_c)
                            {
                                printf("error: Can't copy file to archive.\n");
                                exit_code = 1;
                                break;
                            }
                            size_t -= read_c;
                        }
                    } else
                    {
                        printf("error: %s\n", strerror(errno));
                        exit_code = 1;
                    }
                }
                close(file_d);
                if (exit_code > 0) break;
            }
            free(buffer);
        } else
        {
            printf("error: Not enough memory for buffer. Requested %d bytes\n", BUFFER_SIZE);
            exit_code = 1;
        }

        // Close all descriptors
        close(archive_d);
        closedir(d);

        // Memory free
        for (int i = 0; i < list_size; i++)
        {
            free(file_list[i]->name);
            free(file_list[i]);
        }
        free(file_list);
        free(resolved_dir_path);
        free(resolved_file_path);
        free(resolved_archive_path);
        return exit_code;
    }
}