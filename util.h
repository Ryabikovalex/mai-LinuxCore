#ifndef OSLINUX_UTIL_H
#define OSLINUX_UTIL_H

void safe_realloc(void** ptr,unsigned int size);
// Get size of file;
unsigned long get_fsize(char *file_path);

struct List{
    struct List *prev;
    void *data;
    struct List *next;
};

struct c_file * getFilesListFromArchive(int archive_fd, int *files_count);

#endif //OSLINUX_UTIL_H
