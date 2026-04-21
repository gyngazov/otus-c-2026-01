#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUF_LEN 4096

struct LogLine {
    int size;
    char *url;
    char *ref;
};

struct LogLine parse_line(const char *buf);
int shift(const char *buf, int i, const int limit);

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("no file name\n");
        return EXIT_FAILURE;
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    char buf[BUF_LEN];
    int sum = 0;
    int l;
    struct LogLine ll;

    while (fgets(buf, BUF_LEN, fp) != NULL) {
        ll = parse_line(buf);
    }
    return 0;
}


struct LogLine parse_line(const char *buf)
{
    const int len = strlen(buf);
    const int start_url = shift(buf, 0, len);
    const int end_url = shift(buf, start_url, len) - 2;
    int i = end_url + 7;
    int t = 0;
    while(buf[i] != ' ')
        t = t * 10 + (buf[i++] - '0');
    const int start_ref = i + 2;
    const int end_ref = shift(buf, start_ref, len) - 2;
    struct LogLine ll;
    ll.size = t;
    int u = end_url - start_url + 1;
    int r = end_ref - start_ref + 1;
    char buf_url[u + 1];
    char buf_ref[r + 1];
    memcpy(buf_url, buf + start_url, u);
    memcpy(buf_ref, buf + start_ref, r);
    buf_url[u] = '\0';
    buf_ref[r] = '\0';
    ll.url = buf_url;
    ll.ref = buf_ref;
    
    printf("|%d|%s|%s|\n", ll.size, ll.url, ll.ref);
    return ll;
}

int shift(const char *buf, int i, const int limit)
{
    while(i < limit && buf[i++] != '"');
    return i;
}

