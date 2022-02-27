#ifndef OSLINUX_UTIL_H
#define OSLINUX_UTIL_H

#define DIR_DEPTH 3

void safe_realloc(void** ptr,unsigned int size);
// Get size of file;
unsigned long get_file_size(char *file_path);

struct c_file * get_files_list_from_archive(int archive_fd, int *files_count);

int accumulate_files_from_dir( struct c_file **accumulate, size_t *acc_size, const char *dir_path, const int *depth);

#endif //OSLINUX_UTIL_H
