#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "copy.h"

int isdir(char* filename)
{
    struct stat st;
    errno = 0;

    if (stat(filename, &st) == -1)
        return -1;
    
    return S_ISDIR(st.st_mode);
}

int isempty(char* dir_name)
{
    DIR* dir;
    struct dirent* entry;
    int ret = 0;
    
    if (INV_STR(dir_name))
        return -1;

    dir = opendir(dir_name);
    if (!dir)
        return -1;
    
    errno = 0;
    entry = readdir(dir);
    if (!entry) {
        if (errno)
            ret = -1;
        else
            ret = 1;
    }

    closedir(dir);
    return ret;
}

char* filename_from_path(char* path)
{
    char* filename;

    if (INV_STR(path))
        return NULL;

    if (!(filename = strrchr(path, '/')))
        filename = path;
    else
        filename++;
    
    return filename;
}

int adjust_dir_name(char* dir)
{
    size_t last;

    if (INV_STR(dir))
        return -1;
    
    last = strlen(dir) - 1;
    
    dir[last] *= (dir[last] != '/');
    return 0;
}

/* This function creates the new path for the "path" file
 * @1 [dir]            : Adjsuted directory name
 * @2 [path]           : Path to the file
 * @<                  : New genereted path
 */ 
char* complete_path(char* dir, char* path)
{
    char* filename;
    char* new_path;
    size_t np_size;

    if (INV_STR(dir) || INV_STR(path) || isdir(dir) != 1)
        return NULL;

    filename = filename_from_path(path);
    if (INV_STR(filename))
        return NULL;
    
    np_size = strlen(dir) + strlen(filename) + 2;
    new_path = malloc(np_size);
    if (!new_path)
        return NULL;
    
    sprintf(new_path, "%s/%s", dir, filename);
    return new_path;
}

mode_t get_mode(char* filename)
{
    struct stat st;

    if (stat(filename, &st) == -1)
        return -1;
    else
        return st.st_mode;
}

// Vulnerable to invalid strings. It's better to always use the copy() wrapper
int _copy(char* from_path, char* to_path)
{
    int from, to;
    char buffer[BUF_LEN];
    ssize_t numr;
    mode_t mode;
    
    from = open(from_path, O_RDONLY);
    if (from == -1)
        return -1;
    
    mode = get_mode(from_path);
    if (mode == (mode_t)(-1))
        mode = 0644;
    
    to = open(to_path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (to == -1)
        goto exit_close_from;
    
    while ((numr = read(from, buffer, BUF_LEN)) > 0) {
        if (write(to, buffer, numr) < numr) {
            close(to);
            unlink(to_path);
            goto exit_close_from;
        }
    }
    
    close(to);

exit_close_from:
    return close(from);
}

int copy(char* from_path, char* to_path)
{
    int status = 0;

    if (INV_STR(from_path) || INV_STR(to_path))
        return -1;
    
    if (isdir(from_path) != 0)
        return -1;
    
    switch (isdir(to_path)) {
    case 0:     // Exists and is a file
    case -1:    // Doesn't exist
        if (errno != ENOENT)
            return -1;
        status = _copy(from_path, to_path);
        break;
        
    case 1:     // Exists and is a directory
        if (adjust_dir_name(to_path) == -1)
            return -1;
        
        to_path = complete_path(to_path, from_path);
        if (!to_path)
            return -1;
        
        status = _copy(from_path, to_path);
        free(to_path);
        break;

    default:
        status = -1;
    }

    return status;
}

int _free_list(char** list, size_t list_len)
{
    if (!list)
        return -1;
    
    if (*list)
        for (size_t i = 0; i < list_len; i++)
            free(list[i]);
    else
        return -1;

    return 0;
}

// From path becomes to path
// To path must exist
int r_copy(char* from_path, char* to_path)
{
    int status[2];
    int ret = 0;
    DIR* dir;
    struct dirent* file;
    char* new_to_path, *new_from_path;

    if (INV_STR(from_path) || INV_STR(to_path))
        return -1;
    
    status[0] = isdir(from_path);
    status[1] = isdir(to_path);

    if (status[1] == -1 && errno == ENOENT)
        status[1] = status[0];

    if (status[0] == -1 || status[0] != status[1])
        return -1;
    
    if (status[0] == 0) {
        ret = copy(from_path, to_path);
    } else {
        if (mkdir(to_path, 0766) == -1 && errno != EEXIST)
            return -1;
        
        dir = opendir(from_path);
        if (!dir)
            return -1;
        
        errno = 0;
        while ((file = readdir(dir)) != NULL) {
            if (!strcmp(".", file->d_name) || !strcmp("..", file->d_name))
                continue;
            
            new_to_path = complete_path(to_path, file->d_name);
            if (!new_to_path)
                return -1;
            
            new_from_path = complete_path(from_path, file->d_name);
            if (!new_from_path) {
                free(new_to_path);
                return -1;
            }
            
            ret += r_copy(new_from_path, new_to_path);

            free(new_from_path);
            free(new_to_path);
        }

        closedir(dir);

        if (errno != 0)
            return -1;
    }

    return ret;
}