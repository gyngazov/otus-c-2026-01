#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main() 
{
    struct dirent *de;
    DIR *dr = opendir("xs");
    int i = 0;
    int len;
    char *files[4];
    while ((de = readdir(dr)) != NULL) {
        len = strlen(de->d_name) + 1;
        files[i] = (char *) malloc(len);
        strncpy(files[i], de->d_name, len);
        i++;
    }
    closedir(dr);
    for (int i = 0; i < 4; i++)
        printf("%s\n", files[i]);
    for (int i = 0; i < 4; i++)
        free(files[i]);
    return i;
}

