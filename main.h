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

// Constants
#define DIR_DEPTH 3
#define BUFFER_SIZE 64//1024
#define STR_MAX_SIZE 255 // It's override default value
#define RECORD_SEPARATOR  0x1E
#define BINARY_SEPARATOR  0x1C

// User defined functions
#include "util.h"

// Structs
struct c_file
{
    char *name;
    u_int64_t size;
};

// Function for creating archive
int pack(char *dir_name, char *archive_name);
int unpack(char *archive_name, char *dir_name);
int list(char *archive_name);