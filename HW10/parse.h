#define BUF_LEN 8192

// данные по строке лога
struct LogLine {
    int size;   // пэй лоад запроса
    char *url;  // метод-путь-протокол запроса
    char *ref;  // реферер
};

struct LogLine *parse_line(const char *buf);