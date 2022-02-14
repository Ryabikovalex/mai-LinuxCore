#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("error: Missing arguments\n");
        return -1;
    } else
    {
        if (strcmp(argv[1], "pack") == 0)
        {
            char *archive_name = (argc == 4) ? argv[3] : "archive.custom";
            return pack(argv[2], archive_name);
        }else return 0;
    }
}

unsigned long get_fsize(char *file_path)
{
    struct stat st;
    stat(file_path, &st);
    off_t size = st.st_size;

    return size;
}