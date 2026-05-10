#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#define SIZE                2048
#define MAX_EPOLL_EVENTS    128
#define BACKLOG             128
#define METHOD              "GET /"
#define H403                "403 Forbidden"
#define H404                "404 Not Found"
#define H405                "405 Method not allowed"
#define H500                "500 Internal Server Error"
#define ERRRESP             "HTTP/1.1 %s\r\nConnection: close\r\n"
#define FILERESP            "HTTP/1.1 200 OK\r\n" \
                            "Connection: close\r\n" \
                            "Content-Type: text/html\r\n" \
                            "Content-Length: %lu\r\n" \
                            "\r\n"

static struct epoll_event events[MAX_EPOLL_EVENTS];
static char buffer[SIZE];
static char *file_dir = "./";


int setnonblocking(int sock)
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

int do_read(int fd)
{
    const int rc = recv(fd, buffer, sizeof(buffer), 0);
    if (rc == -1) {
        perror("Ошибка чтения сокета");
        return errno;
    }
    buffer[rc] = 0;
    printf("read: %s\n", buffer);
    return EXIT_SUCCESS;
}

static char *substring(const char *buf, const int start, const int end)
{
    const int bl = strlen(buf) + 1;
    if (end > bl || start > bl || end <= start || end <= 0 || start < 0)
        return NULL;
    const int len = end - start + 1;
    char *res = (char *) malloc(len + 1);
    if (res == NULL)
        return NULL;
    memcpy(res, buf + start, len);
    *(res + len) = '\0';
    return res;
}

void send_file(int sockfd) {
    const char method[] = METHOD;
    char resp[1024];
    int i = 0;
    const int ml = strlen(METHOD);
    for (; i < ml; i++) {
        if (buffer[i] != method[i]) {
            sprintf(resp, ERRRESP, H405);
            if (send(sockfd, resp, strlen(resp), 0) == -1) {
                perror("Ошибка ответа");
                return;
            }
            return;
        }
    }
    for (; buffer[i] != ' ' && i < strlen(buffer); i++);
    char *path = substring(buffer, ml, i - 1);
    if (path == NULL)
        return;
    
    sprintf(resp, "%s%s%s", file_dir, "/", path);
    free(path);
    FILE *fp = fopen(resp, "r");
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
        sprintf(resp, ERRRESP, err);
        send(sockfd, resp, strlen(resp), 0);
        return;
    }
    fseek(fp, 0L, SEEK_END);
    long flen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    sprintf(resp, FILERESP, flen);
    send(sockfd, resp, strlen(resp), 0);
    int n;
    char data[SIZE] = {0};
    while (!feof(fp)) {
        n = fread(data, 1, SIZE, fp);
        if (send(sockfd, data, n, 0) == -1) {
            perror("Error in sending file");
            fclose(fp);
            exit(errno);
        }
    }
    fclose(fp);
}

void process_error(int fd)
{
    printf("fd %d error!\n", fd);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    file_dir = argv[2];
    char* p;
    const int port = strtol(argv[1], &p, 10);
    if (*p) {
        puts("Ошибка номера порта");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("Ошибка сигнала");
        exit(errno);
    }
    const int efd = epoll_create(MAX_EPOLL_EVENTS);
    if (efd == -1) {
        perror("Ошибка создания шины");
        exit(errno);
    }
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Ошибка создания сокета");
        exit(errno);
    }
    if (setnonblocking(listenfd) != EXIT_SUCCESS) {
        close(listenfd);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Не связан адрес");
        exit(errno);
    }
    if (listen(listenfd, BACKLOG) < 0) {
        perror("Ошибка прослушки");   
        exit(errno);
    }
    struct epoll_event listenev;
    listenev.events = EPOLLIN | EPOLLET;
    listenev.data.fd = listenfd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &listenev) < 0) {
        perror("Не сконфигурирована шина");
        exit(errno);
    }
    struct epoll_event connev;
    int events_count = 1;
    int nfds;
    for (;;) {
        nfds = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);
        if (nfds == -1) {
            perror("Ошибка ожидания");
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                int connfd = accept(listenfd, NULL, NULL);
                if (connfd < 0) {
                    perror("Ошибка приема");
                    continue;
                }
                if (events_count == MAX_EPOLL_EVENTS - 1) {
                    puts("Превышение лимита событий");
                    close(connfd);
                    continue;
                }
                if (setnonblocking(connfd) != EXIT_SUCCESS) {
                    close(connfd);
                    continue;
                }
                connev.data.fd = connfd;
                connev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
                if (epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &connev) < 0) {
                    perror("Не сконфигурирована шина");
                    close(connfd);
                    continue;
                }
                events_count++;
            } else {
                int fd = events[i].data.fd;
                if (events[i].events & EPOLLIN)
                    do_read(fd);
                if (events[i].events & EPOLLOUT)
                    send_file(fd);
                if (events[i].events & EPOLLRDHUP)
                    process_error(fd);
                if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, &connev) == -1) {
                    perror("Не сконфигурирована шина");
                    continue;
                }
                close(fd);
                events_count--;
            }
        }
    }
    return EXIT_SUCCESS;
}
