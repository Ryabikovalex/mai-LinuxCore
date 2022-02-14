#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
// POSIX
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
// Pthread
#include <pthread.h>

// User defined functions
#include "util.h"

// Constants
#define BUFFER_SIZE 20//1024
#define NAME_MAX 255 // It's override default value
#define RECORD_SEPARATOR  0x1E
#define BINARY_SEPARATOR  0x1C

// Structs
struct c_file
{
    char *name;
    u_int64_t size;
};

// Function for creating archive
int pack(char *dir_name, char *archive_name);
int unpack(char *archive_name, char *dir_name);

unsigned long get_fsize(char *file_path);