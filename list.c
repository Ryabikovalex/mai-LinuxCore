#include "main.h"

int list(char *archive_name)
{
    int exit_code = 0;
    char *resolved_file_path = calloc(sizeof(char), NAME_MAX + 1);
    realpath(archive_name, resolved_file_path);

    int file_d = open(resolved_file_path, O_RDONLY);
    if (file_d < 0)
    {
        printf("error: %s\n", strerror(errno));
        exit_code = 1;
    } else
    {
        void *buffer = calloc(1, BUFFER_SIZE);
        char *filename = calloc(sizeof(char), NAME_MAX+1);
        u_int64_t read_c = 0;
        u_int64_t buff_c = 0;
        char char_t = 0;
        if (buffer == NULL || filename == NULL)
        {
            printf("error: Not enough memory. Requested %d bytes\n", BUFFER_SIZE+NAME_MAX+1);
            exit_code = 1;
        } else
        {
            if (read(file_d, (void *) &char_t, sizeof(char)) != 1)
            {
                printf("error: %s\n", strerror(errno));
                exit_code = 1;
            } else {
                if (char_t != RECORD_SEPARATOR)
                {
                    printf("error: Not archive\n");
                    exit_code = 1;
                } else {
                    // Rewind
                    read_c = 0;
                    lseek(file_d, (long)read_c, SEEK_SET);
                    read_c += read(file_d, (void *) &char_t, sizeof(char));
                    while (char_t == RECORD_SEPARATOR)
                    {
                        memset(filename, 0, NAME_MAX+1);
                        u_int64_t size = 0;
                        u_int64_t position = 0;
                        size_t buff_pos = 0;
                        // Read offset
                        read_c += read(file_d, buffer, sizeof(u_int64_t));
                        memcpy(&position, buffer, sizeof(u_int64_t));
                        // Read size
                        read_c += read(file_d, buffer, sizeof(u_int64_t));
                        memcpy(&size, buffer, sizeof(u_int64_t));

                        // Read filename
                        buff_c = read(file_d, buffer, BUFFER_SIZE);
                        read_c += buff_c;

                        size_t str_len = strlen(filename);
                        void *temp_a = memchr(buffer, RECORD_SEPARATOR, buff_c);
                        void *temp_b = memchr(buffer, BINARY_SEPARATOR, buff_c);
                        void *temp = (temp_a == NULL) ? temp_b : temp_a;
                        while (temp == NULL)
                        {
                            memcpy(filename+str_len, buffer, buff_c);
                            filename[str_len+buff_c] = 0;
                            str_len = strlen(filename);
                            buff_c = read(file_d, buffer, BUFFER_SIZE);
                            read_c += buff_c;
                            temp_a = memchr(buffer, RECORD_SEPARATOR, buff_c);
                            temp_b = memchr(buffer, BINARY_SEPARATOR, buff_c);
                            temp = (temp_a == NULL) ? temp_b : temp_a;
                        }
                        memcpy(filename+str_len, buffer, temp - buffer);
                        filename[str_len+(temp - buffer)] = 0;

                        // Print information
                        printf("File: %s size: %lu bytes, offset: %lx\n", filename, size, position);

                        // TODO
                        // Set to next record
                        read_c -= buff_c - (temp - buffer);
                        lseek(file_d, (long)read_c, SEEK_SET);
                        read_c += read(file_d, (void *) &char_t, sizeof(char));
                    }
                }
            }
        }
        free(filename);
        free(buffer);
        close(file_d);
    }

    free(resolved_file_path);
    return exit_code;
}