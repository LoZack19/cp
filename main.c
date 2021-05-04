#include "copy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "pathresolve.h"

#define F_RECURSIVE     0x01

#define __IS_FLAG_SET(field, flag)    ((field) & (flag))
#define SET_FLAG(field, flag)         ((field) |= (flag))
#define IS_RECURSIVE(field)           (__IS_FLAG_SET((field), (F_RECURSIVE)))

void help()
{
    return;
}

int main(int argc, char* argv[])
{
    int opt;
    uint8_t flags = 0;
    int status[2];
    char* from_path;
    char* to_path;
    int ret = 0;

    while ((opt = getopt(argc, argv, "rh")) != -1) {
        switch (opt)
        {
        case 'r':
            SET_FLAG(flags, F_RECURSIVE);
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
        fputs("Failed to resolve source path\n", stderr);
        ret = -1;
        goto free_from_path;
    }

    printf("from:\t%s\nto:\t%s\nflags:\t%x\n", from_path, to_path, flags);

    status[0] = isdir(from_path);
    if (status[0] == -1) {
        if (errno == ENOENT)
            fprintf(stderr, "No entry found for %s: %s\n", from_path, strerror(errno));
        else
            perror("Couldn't open the source to copy from");
        
        ret = -1;
        goto free_to_path;
    }
    
    status[1] = isdir(to_path);
    if (status[1] == -1 && errno != ENOENT) {
        perror("Failed to copy to destination");
        exit(-1);
    } else if (errno == ENOENT) {
        char* tmp = strdup(to_path);
        if (!tmp) {
            fputs("Wrong destination\n", stderr);
            ret = -1;
            goto free_to_path;
        }
        
        char* prc = strrchr(tmp, '/');
        if (!prc) {
            fputs("Wrong destination\n", stderr);
            free(tmp);
            ret = -1;
            goto free_to_path;
        }

        *prc = '\0';
        if (isdir(tmp) != 1) {
            fputs("Wrong destination\n", stderr);
            free(tmp);
            ret = -1;
            goto free_to_path;
        }

        free(tmp);
    }

    if (!IS_RECURSIVE(flags) && status[0] == 1) {
        fprintf(stderr, "%s is a directory\n", from_path);
        ret = -1;
        goto free_to_path;
    }

free_to_path:
    free(from_path);
free_from_path:
    free(to_path);

    return ret;
}