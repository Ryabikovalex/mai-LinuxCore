#ifndef OSLINUX_UTIL_H
#define OSLINUX_UTIL_H

void safe_realloc(void** ptr,unsigned int size);
// Get size of file;
unsigned long get_file_size(char *file_path);

int mkdir_p_flag(char* file_path, mode_t mode);

struct c_file * get_files_list_from_archive(int archive_fd, int *files_count);

int accumulate_files_from_dir(const char *dir_path, char **acc_path, struct c_file ***accumulate,
        u_int32_t *acc_size, int *depth);

#endif //OSLINUX_UTIL_H
