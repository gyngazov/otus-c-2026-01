#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main() 
{
    struct dirent *de;
    DIR *dr = opendir("xs");
    if (dr == NULL) {
        perror("Не открылась папка");
        exit(errno);
    }
    int i = 0;
    int len;
    char *files[4];
    while ((de = readdir(dr)) != NULL) {
        len = strlen(de->d_name) + 1;
        files[i] = (char *) malloc(len);
        if (files[i] == NULL) {
            puts("Не выделена память");
            exit(EXIT_FAILURE);
        }
        if (strncpy(files[i], de->d_name, len) == -1) {
            perror("Ошибка копирования");
            exit(errno);
        }
        i++;
    }
    closedir(dr);
    for (int i = 0; i < 4; i++)
        puts(files[i]);
    for (int i = 0; i < 4; i++)
        free(files[i]);
    return i;
}

