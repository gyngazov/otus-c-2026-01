#include <stdio.h>
#include <string.h>

#define BUF_LEN 1024

int get_load(char *buf, int len);

int main()
{
    FILE *fp;
    fp = fopen("a.log", "r");
    char buf[BUF_LEN];
    while (fgets(buf, BUF_LEN, fp) != NULL) {
        printf("%d : %s", get_load(buf, strlen(buf)), buf);
    }
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