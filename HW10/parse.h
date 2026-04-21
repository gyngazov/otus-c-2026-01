#define BUF_LEN 4096

struct LogLine {
    int size;
    char *url;
    char *ref;
};

struct LogLine parse_line(const char *buf);