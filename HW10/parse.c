#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "parse.h"

static int shift(const char *buf, int i, const int limit)
{
    while(i < limit && buf[i++] != '"');
    return i;
}

static char *get_str(const char *buf, const int start, const int end)
{
    const int len = end - start + 1;
    char *url = (char *) malloc(len + 1);
    if (url == NULL) {
        perror("Не выделена память");
        exit(errno);
    }
    memcpy(url, buf + start, len);
    *(url + len) = '\0';
    return url;
}

struct LogLine *parse_line(const char *buf)
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
    struct LogLine *ll = (struct LogLine *) malloc(sizeof(struct LogLine));
    if (ll == NULL)
        return NULL;
    ll->size = t;
    ll->url = get_str(buf, start_url, end_url);
    ll->ref = get_str(buf, start_ref, end_ref);
    return ll;
}



