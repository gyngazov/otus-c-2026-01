#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "parse.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("no file name\n");
        return EXIT_FAILURE;
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    char buf[BUF_LEN];
    struct LogLine ll;
    init();
    while (fgets(buf, BUF_LEN, fp) != NULL) {
        ll = parse_line(buf);
        inc(ll.ref);
    }
    fclose(fp);
    get_top_n(10);
    destroy();

    return 0;
    
}