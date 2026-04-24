#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "hash.h"
#include "parse.h"



int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("no file name\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Нельзя открыть файл\n");
        exit(errno);
    }
    char buf[BUF_LEN];
    struct LogLine *ll;
    GHashTable *h = init();
    while (fgets(buf, BUF_LEN, fp) != NULL) {
        ll = parse_line(buf);
        inc(h, ll->ref, 1);
    }
    fclose(fp);
    get_top(h);
    printf("s=%d\n", sum(h));
    destroy(h);
    return 0;
    
}