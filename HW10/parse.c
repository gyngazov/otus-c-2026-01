#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_LEN 4096
#define REF_LEN 1024

struct LogLine {
    int size;
    char url[BUF_LEN];
    char ref[BUF_LEN];
};

int get_load(char *buf, int len);
struct LogLine parse_line(char *buf);
int shift(char *buf, int i, int limit);

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

    while (fgets(buf, BUF_LEN, fp) != NULL)
        ll = parse_line(buf);
    return 0;
}

int get_load(char *buf, int len)
{
    int blanks = 0;
    int t = 0;
    char c;
    for (int i = 0; i < len; i++) {
        c = buf[i];
        if (c == ' ')
            blanks += 1;
        else if (blanks == 9)
            t = t * 10 + (c - '0');        
        else if (blanks == 10)
            break;
    }
    return t;
}

struct LogLine parse_line(char *buf)
{
    int len = strlen(buf);
    int start_url = shift(buf, 0, len);
    int end_url = shift(buf, start_url, len) - 2;
    int i = end_url + 7;
    int t = 0;
    while(buf[i] != ' ') {
        t = t * 10 + (buf[i] - '0');
        i++;
    }
    int start_ref = i + 2;
    int end_ref = shift(buf, start_ref, len) - 2;
    struct LogLine ll;
    ll.size = t;
    strncpy(ll.url, buf + start_url, end_url - start_url + 1);
    strncpy(ll.ref, buf + start_ref, end_ref - start_ref + 1);
    printf("%d %s %s\n", ll.size, ll.url, ll.ref);
    return ll;
}

int shift(char *buf, int i, int limit)
{
    while(i < limit && buf[i++] != '"');
    return i;
}