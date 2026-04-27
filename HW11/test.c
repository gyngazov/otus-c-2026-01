#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#define PORT        23
#define BUF_SIZE    16384
#define ANSWER      "Press control-C to interrupt any command."
#define IP          "64.13.139.230"
#define CMD_SIZE    1024
#define FIGLET      "figlet"
#define FORMAT      "%s /%s %s\n\r"
#define USAGE       "Usage: %s -f <font> -m <message>\n"
#define TIMEOUT_SEC 5


int main(int argc, char** argv)
{
    if (argc == 1) {
        printf(USAGE, argv[0]); 
        exit(EXIT_FAILURE);
    }
    char *font;
    char *msg;
    int opt;
    extern char *optarg;
    while((opt = getopt(argc, argv, ":f:m:")) != -1) { 
        switch(opt) {  
            case 'f': 
                font = optarg;
                break;
            case 'm': 
                msg = optarg;
                break;
            case ':': 
            case 'h':
            case '?': 
            default:
                printf(USAGE, argv[0]); 
                exit(EXIT_FAILURE);
        } 
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0){
        perror("socket");
        exit(errno);
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("Не установлен таймаут");
		close(sock_fd);
        exit(errno);
	}

    struct sockaddr_in sock_addr = {0};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);

    if(inet_pton(PF_INET, IP, &sock_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        exit(errno);
    }
    if(connect(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0){
        perror("connect");
        close(sock_fd);
        exit(errno);
    }
    char buffer[BUF_SIZE];
    while (recv(sock_fd, buffer, BUF_SIZE - 1, 0) > 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("Таймаут\n");
            exit(EXIT_FAILURE);
        } else if (strstr(buffer, ANSWER) != NULL) {
			break;
        }
    }

    char command[CMD_SIZE];
	if (snprintf(command, CMD_SIZE, FORMAT, FIGLET, font, msg) < 0) {
		printf("Не формируется строка\n");
		exit(EXIT_FAILURE);
	}

    if(send(sock_fd, command, strlen(command), 0) < 0) {
        perror("send");
        close(sock_fd);
        exit(errno);
    }

    int r;
	while ((r = recv(sock_fd, buffer, BUF_SIZE, 0)) > 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("Таймаут\n");
            exit(EXIT_FAILURE);
        }
        buffer[r] = '\0';
        if (strstr(buffer, "\n") != NULL)
            break;
    }
    printf("%s\n", strstr(buffer, "\n"));
    if (shutdown(sock_fd, SHUT_RDWR) == -1) {
        perror("shut down");
        close(sock_fd);
        exit(errno);
    }
    close(sock_fd);
    return EXIT_SUCCESS;
}
