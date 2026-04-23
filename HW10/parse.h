#define BUF_LEN 8192

struct LogLine {
    int size;
    char *url;
    char *ref;
};

struct LogLine *parse_line(const char *buf);

#define PRCO printf("%d\n", __COUNTER__);