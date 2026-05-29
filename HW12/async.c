#define _GNU_SOURCE

#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>

#define SIZE                2048
#define METHOD              "GET /"
#define H403                "403 Forbidden"
#define H404                "404 Not Found"
#define H405                "405 Method not allowed"
#define H500                "500 Internal Server Error"
#define ERRRESP             "HTTP/1.1 %s\r\nConnection: close\r\n"
#define FILERESP            "HTTP/1.1 200 OK\r\n" \
                            "Connection: close\r\n" \
                            "Content-Type: text/html\r\n" \
                            "Content-Length: %s\r\n" \
                            "\r\n"

int setnonblocking(const int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        perror("Нет статус флагов");
        return errno;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock, F_SETFL, opts) < 0) {
        perror("F_SETFL не установлено");
        return errno;
    }
    return EXIT_SUCCESS;
}

char *substring(const char *buf, const int start, const int end)
{
    const int bl = strlen(buf) + 1;
    if (end > bl || start > bl || end <= start || end <= 0 || start < 0)
        return NULL;
    const int len = end - start + 1;
    char *res = (char *) malloc(len + 1);
    if (res == NULL)
        return NULL;
    memcpy(res, buf + start, len);
    res[len] = '\0';
    return res;
}

static int snprint_send(const int sockfd, const char *mask, const char* msg)
{
    char resp[SIZE];
    if (snprintf(resp, SIZE, mask, msg) == -1) {
        puts("Ошибка копирования");
        return -1;
    }
    if (send(sockfd, resp, strlen(resp), 0) == -1) {
        perror("Ошибка ответа");
        return -2;
    }
    return 0;
}

// ответить файлом или ошибкой
void send_file(const int sockfd, const char *file_path) 
{
    if (file_path == NULL) {
        snprint_send(sockfd, ERRRESP, H500);
        return;
    }
    FILE *fp = fopen(file_path, "r");
    char *err;
    if (fp == NULL) {
        switch (errno) {
            case 13:
                err = H403;
                break;
            case 2:
                err = H404;
                break;
            default:
                err = H500;
                break;
        }
        snprint_send(sockfd, ERRRESP, err);
        return;
    }
    char *err_txt = "Ошибка файла";
    if (fseek(fp, 0L, SEEK_END) == -1)
        goto end;
    long flen = ftell(fp);
    if (flen == -1 || fseek(fp, 0L, SEEK_SET) == -1)
        goto end;
    char tmp[16];
    if (snprintf(tmp, 16, "%lu", flen) == -1) {
        err_txt = "Ошибка копирования";
        goto end;
    }
    if (snprint_send(sockfd, FILERESP, tmp) < 0) {
        err_txt = "Ошибка отправки";
        goto end;
    }
    int n;
    char resp[SIZE];
    while (!feof(fp)) {
        n = fread(resp, 1, SIZE, fp);
        if (send(sockfd, resp, n, 0) == -1)
            goto end;
    }
    return;
end:
    perror(err_txt);
    fclose(fp);
}

void process_error(int fd)
{
    printf("fd %d error!\n", fd);
}

void ignore_broken_pipe()
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);     
    sa.sa_flags = 0;              

    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("Ошибка сигнала");
        exit(errno);
    }
}