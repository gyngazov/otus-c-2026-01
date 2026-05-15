#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "async.h"

#define MAX_EPOLL_EVENTS    128
#define BACKLOG             128
#define SIZE                2048
#define METHOD              "GET /"

static char buffer[SIZE];

struct epoll_event events[MAX_EPOLL_EVENTS];
char *file_dir = "./";

int set_options(int argc, char** argv);
int do_read(const int fd);
char *set_file();

int main(int argc, char** argv)
{
    const int port = set_options(argc, argv);
    #include <signal.h>

    ignore_broken_pipe();
    const int efd = epoll_create(MAX_EPOLL_EVENTS);
    if (efd == -1) {
        perror("Ошибка создания шины");
        exit(errno);
    }
    const int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Ошибка создания сокета");
        exit(errno);
    }
    if (setnonblocking(listenfd) != EXIT_SUCCESS)
        goto err;
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    char *err_txt = "Не связан адрес";
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        goto err;
    if (listen(listenfd, BACKLOG) < 0) {
        err_txt = "Ошибка прослушки";   
        goto err;
    }
    struct epoll_event listenev;
    listenev.events = EPOLLIN | EPOLLET;
    listenev.data.fd = listenfd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &listenev) < 0) {
        err_txt = "Не сконфигурирована шина";
        goto err;
    }
    struct epoll_event connev;
    int events_count = 1;
    int nfds, connfd, fd;
    char *file_path;
    for (;;) {
        nfds = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);
        if (nfds == -1) {
            perror("Ошибка ожидания");
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                connfd = accept(listenfd, NULL, NULL);
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
                fd = events[i].data.fd;
                if (events[i].events & EPOLLIN)
                    do_read(fd);
                
                if (events[i].events & EPOLLOUT) {
                    file_path = set_file();
                    send_file(fd, file_path);
                }
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
    err:
    perror(err_txt);
    close(listenfd);
    return errno;
}

#define DIE(argv0) do { \
    printf("usage: %s -p <port> -d <file dir>\n", argv0); \
    exit(EXIT_FAILURE); \
} while (0)

int getopt(int argc, char **argv, const char *params);

int set_options(int argc, char** argv)
{
    if (argc == 1) 
        DIE(argv[0]);
    int opt;
    long port;
    extern char *optarg;
    extern int optind;
    int i = 2;
    while((opt = getopt(argc, argv, ":d:p:")) != -1) { 
        switch(opt) {  
            case 'd': 
                file_dir = optarg;
                i--;
                break;
            case 'p': 
                port = strtol(optarg, NULL, 10);
                i--;
                break;
            case ':': 
                DIE(argv[0]);
            case 'h':
            case '?': 
            default:
                DIE(argv[0]);
        } 
    }
    if (optind < argc 
        || port <= 0 
        || strlen(file_dir) == 0 
        || i != 0 
        || errno != 0 
        || port > 65535)
        DIE(argv[0]);
    return port;
}

// путь к файлу по строке запроса
char *set_file() 
{
    const char method[] = METHOD;
    int i = 0;
    const int ml = strlen(METHOD);
    const int bl = strlen(buffer);
    for (; i < ml; i++)
        if (buffer[i] != method[i])
            return NULL;
    for (; buffer[i] != ' ' && i < bl; i++);
    char *path = substring(buffer, ml, i - 1);
    if (path == NULL)
        return NULL;
    int len = strlen(file_dir) + 1 + strlen(path) + 1;
    char *file_path = (char *) malloc(len + 1);
    if (file_path == NULL)
        return NULL;
    if (snprintf(file_path, len, "%s%s%s", file_dir, "/", path) == -1) {
        puts("Ошибка копирования");
        free(path);
        free(file_path);
        return NULL;
    }
    file_path[len] = 0;
    free(path);
    return file_path;
}

int do_read(const int fd)
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

