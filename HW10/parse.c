#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"

static int shift(const char *buf, int i, const int limit)
{
    while(i < limit && buf[i++] != '"');
    return i;
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
    ll->size = t;
    const int u = end_url - start_url + 1;
    const int r = end_ref - start_ref + 1;
    char *url = (char *) malloc(u + 1);
    memcpy(url, buf + start_url, u);
    *(url + u) = '\0';
    ll->url = url;
    char *ref = (char *) malloc(r + 1);
    memcpy(ref, buf + start_ref, r);
    *(ref + r) = '\0';
    ll->ref = ref;
    
    printf("|%d|%s|%s|\n", ll->size, ll->url, ll->ref);
    return ll;
}


