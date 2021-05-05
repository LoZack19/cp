#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <errno.h>
#include <unistd.h>
#include <getopt.h>

#include "copy.h"
#include "pathresolve.h"

#define F_RECURSIVE     0x01
#define F_FORCED        0x02
#define F_DIRECTORY     0x04

#define __IS_FLAG_SET(field, flag)    ((field) & (flag))
#define SET_FLAG(field, flag)         ((field) |= (flag))

#define IS_RECURSIVE(field)           (__IS_FLAG_SET((field), (F_RECURSIVE)))
#define IS_FORCED(field)              (__IS_FLAG_SET((field), (F_FORCED)))
#define IS_DIRECTORY(field)           (__IS_FLAG_SET((field), (F_DIRECTORY)))

void help()
{
    return;
}

char* pathof(char* file)
{
    char* tmp;

    if (INV_STR(file))
        return NULL;
    
    tmp = strdup(file);
    if (!tmp)
        return NULL;
    
    path_kill_back(tmp);
    return tmp;
}

int parse(char* source, char* destination, uint8_t flags)
{
    int status[2];

    if (INV_STR(source) || INV_STR(destination))
        return -1;
    
    status[0] = isdir(source);
    if (status[0] == -1)
        return -1;
    
    status[1] = isdir(destination);
    if (status[1] == -1) {
        if (errno == ENOENT)
            status[1] = status[0] + 2;
        else
            return -1;
    }

    // Check source
    if (status[0] == 1) {
        if (!IS_DIRECTORY(flags)) {
            return -1;
        }

        if (!IS_RECURSIVE(flags)) {
            if (isempty(source) != 1)
                return -1;
        }
    }

    // Check destination
    if (status[1] == 1) {
        return 1; // Resolve and repeat
    } else if (status[1] == 0) {
        if (!IS_FORCED(flags))
            return -1;
    } else if (status[1] == 2 || status[1] == 3) {
        char* tmp = pathof(destination);
        if (!tmp)
            return -1;
        
        if (!isdir(tmp)) {
            free(tmp);
            return -1;
        }
        
        free(tmp);
    }

    return 0;
}

int main_copy(char* source, char* destination, uint8_t flags)
{
    return parse(source, destination, flags);
}

int main(int argc, char* argv[])
{
    int opt;
    uint8_t flags = 0;
    char* from_path;
    char* to_path;
    int ret = 0;

    while ((opt = getopt(argc, argv, "rfdh")) != -1) {
        switch (opt)
        {
        case 'r':
            SET_FLAG(flags, F_RECURSIVE);
            __attribute__((fallthrough));
        case 'd':
            SET_FLAG(flags, F_DIRECTORY);
            break;
        
        case 'f':
            SET_FLAG(flags, F_FORCED);
            break;
        
        case 'h':
        case '?':
            help();
            if (opt == '?' || argc != 2)
                exit(-1);
            else
                exit(0);
        
        default:
            break;
        }
    }

    if (argc < optind + 2) {
        help();
        exit(-1);
    }

    from_path = path_resolve(argv[optind]);
    if(from_path == NULL) {
        fputs("Failed to resolve source path\n", stderr);
        exit(-1);
    }
    to_path = path_resolve(argv[optind + 1]);
    if(to_path == NULL) {
        fputs("Failed to resolve destination path\n", stderr);
        ret = -1;
        goto free_from_path;
    }

    printf("from:\t%s\nto:\t%s\nflags:\t%x\n", from_path, to_path, flags);

    ret = main_copy(from_path, to_path, flags);
    if (ret == -1)
        goto free_to_path;

free_to_path:
    free(from_path);
free_from_path:
    free(to_path);

    return ret;
}