
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

struct Div {
    int nn;
    FILE **fp;
};

struct Div *divide(const int threads_n, const char *dir_name, int *n);

void main(int argc, char **argv)
{   
    int n;
    struct Div *d = divide(12, "xs", &n);
    
    if (d == NULL)
        exit(EXIT_FAILURE);
    for (int i = 0; i < n; i++)
        printf("%d ", (d + i)->nn);
        
}

struct Div *divide(const int threads_n, const char *dir_name, int *m)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Failed to open directory");
        return 1;
    }
    int k = 0;
    while (readdir(dir) != NULL)
        k++;
    if (k == 0)
        return NULL;
//         printf("Name: %s, Type: %d
// ", entry->d_name, entry->d_type);
    

    
    const int base = k / threads_n;
    const int rest = k % threads_n;
    const int n = threads_n >= k ? k : threads_n;
    *m = n;
    struct Div *divs = (struct Div *) malloc(n * sizeof(struct Div));
    if (divs == NULL) {
        printf("Нет памяти\n");
        return NULL;
    }
    int i = 0;
    for (; i < rest; i++) {
        k = base + 1;
        while (k-- > 0) {
            entry = readdir(dir);
            
        }
    }
        (divs + i)->nn = base + 1;
    for (; i < n; i++)
        (divs + i)->nn = base;
    closedir(dir);
    return divs;
}
