
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

struct Div {
    int n;
    char **files;
};

struct Div *divide(const int threads_n, const char *dir_name, int *n);
static int count_files(const char *dir_name);
static char **get_files(int num_files, DIR *dir);

void main(int argc, char **argv)
{   
    int n;
    struct Div *divs = divide(3, "xs", &n);
    
    if (divs == NULL)
        exit(EXIT_FAILURE);
    int len;
    for (int i = 0; i < n; i++) {
        len = divs[i].n;
        printf("%d: n->%d fsl->", i, len);
        for (int k = 0; k < len; k++)
            printf("%s ", divs[i].files[k]);
    }
        
}
// тредлист
// распределить файлы по потокам
struct Div *divide(const int threads_n, const char *dir_name, int *m)
{
    DIR *dir;
    struct dirent *entry;

    int k = count_files(dir_name);
    if (k == -1) {
        printf("Ошибка чтения папки\n");
        return NULL;
    }
    const int base = k / threads_n;
    const int rest = k % threads_n;
    const int n = threads_n >= k ? k : threads_n;
    *m = n;
    struct Div *divs = (struct Div *) malloc(n * sizeof(struct Div));
    if (divs == NULL) {
        printf("Нет памяти\n");
        return NULL;
    }
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Папка не доступна");
        return NULL;
    }
    readdir(dir);
    readdir(dir);
    int i = 0;
    for (; i < rest; i++) {
        divs[i].n = base + 1;
        divs[i].files = get_files(base + 1, dir);
    }
    for (; i < n; i++) {
        divs[i].n = base;
        divs[i].files = get_files(base, dir);
    }

    closedir(dir);
    return divs;
}

// штук файлов в папке
static int count_files(const char *dir_name)
{
    DIR *dir;
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Папка не доступна");
        return -1;
    }
    int k = 0;
    readdir(dir); // .
    readdir(dir); // ..
    while (readdir(dir) != NULL)
        k++;
    closedir(dir);
    if (k == 0) {
        printf("Нет файлов.\n");
        return -1;
    }
    return k;
}

// список файлов для потока
static char **get_files(int num_files, DIR *dir)
{
    struct dirent *entry;
    char **fls = malloc(num_files * sizeof(char *));
    for (int j = 0; j < num_files; j++) {
        entry = readdir(dir);
        fls[j] = entry->d_name;
    } 
    return fls;
}


