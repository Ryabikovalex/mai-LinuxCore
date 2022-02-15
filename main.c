#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("error: Missing arguments\n");
        return -1;
    } else
    {
        if (strcmp(argv[1], "list") == 0)
        {
            return list(argv[2]);
        } else if (strcmp(argv[1], "pack") == 0)
        {
            char *archive_name = (argc == 4) ? argv[3] : "archive.custom";
            return pack(argv[2], archive_name);
        } else if (strcmp(argv[1], "unpack") == 0)
        {
            char *output_folder = (argc == 4) ? argv[3] : ".";
            return unpack(argv[2], output_folder);
        } else return 0;
    }
}