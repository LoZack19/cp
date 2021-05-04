#ifndef COPY_H
#define COPY_H

#include <stdlib.h>

#define BUF_LEN     0x50
#define PATH_LEN    0x100

#define INV_STR(STR)    (!(STR) || !(*STR))

int isdir(char* filename);

char* filename_from_path(char* path);

int adjust_dir_name(char* dir);

char* complete_path(char* dir, char* path);

int copy(char* from_path, char* to_path);

int _free_list(char** list, size_t list_len);

int r_copy(char* from_path, char* to_path);


static inline void free_list(char** list, size_t len)
{
    _free_list(list, len);
    free(list);
}

#endif