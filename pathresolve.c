#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <linux/limits.h>

// @vbextreme contribution to Leonia-Tech/texted

// Find how many times the character ch appears in str
static unsigned int strocc(const char* str, char ch)
{
	if(!str)
		return 0;
	
	unsigned int counter = 0;

	for (unsigned int i = 0; str[i] != 0; i++)
		if (str[i] == ch)
			counter++;

	return counter;
}

void path_kill_back(char* path)
{
	size_t len = strlen(path);

	if (len == 1)
        return;
    
	if (path[len-1] == '/')
        path[len-1] = 0;
    
	char* bs = strrchr(path, '/');

	if(bs)
        *bs = 0;
}

static int path_home(char* path)
{
    char *hd;

    if ((hd = getenv("HOME")) == NULL) {
		struct passwd* spwd = getpwuid(getuid());

		if (!spwd) {
			*path = 0;
			return -1;
        }

        strcpy(path, spwd->pw_dir);
    } else {
		strcpy(path, hd);
	}
	return 0;
}

char* path_resolve(const char* path)
{
	char cur[PATH_MAX];
	char out[PATH_MAX];
    unsigned int count;

	size_t lpath = strlen(path);
	if (lpath > PATH_MAX - 1)
        return NULL;
	
	count = strocc(path, '/');

	if ((lpath == 1 && !strncmp(path, "~", 1)) || !strncmp(path, "~/", 2)) {
		if (path_home(cur)) 
            return NULL;
		if  (lpath + strlen(cur) > PATH_MAX -1)
            return NULL;
		if (path[1] && path[2])
			strcpy(&cur[strlen(cur)], &path[1]);
	} else if ((lpath == 1 && !strncmp(path, ".", 1)) || !strncmp(path, "./", 2) || count == 0) {
		getcwd(cur, PATH_MAX);

		if (lpath + strlen(cur) > PATH_MAX - 1)
            return NULL;
        
        if (!count) {
            strcat(cur, "/");
            strcat(cur, path);
        } else if (path[1] && path[2])
			strcpy(&cur[strlen(cur)], &path[1]);
	} else if( (lpath ==  2 && !strncmp(path, "..", 2)) || !strncmp(path, "../", 3)) {
		getcwd(cur, PATH_MAX);

		path_kill_back(cur);

		if (lpath + strlen(cur) > PATH_MAX - 1)
            return NULL;
        
		if (path[2] && path[3])
			strcpy(&cur[strlen(cur)], &path[2]);
	} else {
		strcpy(cur, path);
	}

	char* parse = cur;
	char* pout = out;

	while (*parse) {
		if (*parse == '.') {
			if (*(parse+1) == '/') {
				parse += 2;
				continue;
			}

			if (*(parse+1) == '.') {
				if (*(parse+2) == 0) {
					*pout = 0;
					path_kill_back(out);
					pout = out + strlen(out);
					parse += 2;
					continue;
				}
				if (*(parse+2) == '/') {
					*pout = 0;
					path_kill_back(out);
					pout = out + strlen(out);
					parse += 2;
					continue;
				}
			}
		}
		*pout++ = *parse++;
	}

	*pout = 0;
	return strdup(out);
}