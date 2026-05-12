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

#include "async.h"

#define MAX_EPOLL_EVENTS    128
#define BACKLOG             128
#define SIZE                2048
static char buffer[SIZE];

struct epoll_event events[MAX_EPOLL_EVENTS];
char *file_dir = "./";

int set_options(int argc, char** argv);

int main(int argc, char** argv)
{
    const int port = set_options(argc, argv);
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("Ошибка сигнала");
        exit(errno);
    }
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
    int nfds, connfd, fd;
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
                    do_read(fd, buffer);
                if (events[i].events & EPOLLOUT)
                    send_file(fd, file_dir, buffer);
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

#define DIE(argv0) do { \
    printf("usage: %s -p <port> -d <file dir>\n", argv0); \
    exit(EXIT_FAILURE); \
} while (0)

int getopt(int argc, char **argv, const char *params);

int set_options(int argc, char** argv)
{
    if (argc == 1) 
        DIE(argv[0]);
    int opt, port;
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
                port = atoi(optarg);
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
    if (optind < argc || port == 0 || strlen(file_dir) == 0 || i != 0)
        DIE(argv[0]);
    return port;
}
