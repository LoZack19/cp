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
    to_path = path_resolve(argv[optind + 1]);

    printf("from:\t%s\nto:\t%s\nflags:\t%x\n", from_path, to_path, flags);

    status[0] = isdir(from_path);
    if (status[0] == -1) {
        if (errno == ENOENT)
            fprintf(stderr, "No entry found for %s: %s\n", from_path, strerror(errno));
        else
            perror("Couldn't open the source to copy from");
        
        exit(-1);
    }
    
    status[1] = isdir(to_path);
    if (status[1] == -1 && errno != ENOENT) {
        perror("Failed to copy to destination");
        exit(-1);
    } else if (errno == ENOENT) {
        char* tmp = strdup(to_path);
        if (!tmp) {
            fputs("Wrong destination\n", stderr);
            exit(-1);
        }
        
        char* prc = strrchr(tmp, '/');
        if (!prc) {
            fputs("Wrong destination\n", stderr);
            free(tmp);
            exit(-1);
        }

        *prc = '\0';
        if (isdir(tmp) != 1) {
            fputs("Wrong destination\n", stderr);
            free(tmp);
            exit(-1);
        }

        free(tmp);
    }

    if (!IS_RECURSIVE(flags) && status[0] == 1) {
        fprintf(stderr, "%s is a directory\n", from_path);
        exit(-1);
    }

    return 0;
}