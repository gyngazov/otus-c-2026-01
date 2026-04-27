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
#define FORMAT      "figlet /%s %s\n\r"
#define USAGE       "Usage: %s -f <font> -m <message>\n"
#define FAIL        printf(USAGE, argv[0])

int getopt(int argc, char** argv, char *params);

int main(int argc, char** argv)
{
    if (argc == 1) {
        FAIL;
        exit(EXIT_FAILURE);
    }
    char *font;
    char *msg;
    int opt;
    extern char *optarg;
    extern int optind;
    while((opt = getopt(argc, argv, ":f:m:")) != -1) { 
        switch(opt) {  
            case 'f': 
                font = optarg;
                break;
            case 'm': 
                msg = optarg;
                break;
            case ':': 
                FAIL;
                exit(EXIT_FAILURE);
            case 'h':
            case '?': 
            default:
                FAIL;
                exit(EXIT_FAILURE);
        } 
    }
    if (optind < argc) {
        FAIL;
                exit(EXIT_FAILURE);
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0){
        perror("socket");
        exit(errno);
    }

    struct sockaddr_in sock_addr = {0};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);

    int exit_code = EXIT_SUCCESS;
    char *err_msg = "";

    if (inet_pton(PF_INET, IP, &sock_addr.sin_addr) <= 0) {
        err_msg = "Не установлен адрес";
        exit_code = errno;
        goto err;
    }
    if (connect(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0){
        err_msg = "Не установлено соединение";
        exit_code = errno;
        goto err;
    }
    char buffer[BUF_SIZE];
    while (recv(sock_fd, buffer, BUF_SIZE - 1, 0) > 0) {
        if (strstr(buffer, ANSWER) != NULL)
			break;
    }

    char command[CMD_SIZE];
	if (snprintf(command, CMD_SIZE, FORMAT, font, msg) < 0) {
		printf("Не формируется команда\n");
		exit(EXIT_FAILURE);
	}

    if (send(sock_fd, command, strlen(command), 0) < 0) {
        err_msg = "Ошибка отправки";
        exit_code = errno;
        goto err;
    }

    int r;
	while ((r = recv(sock_fd, buffer, BUF_SIZE, 0)) > 0) {
        buffer[r] = '\0';
        if (strstr(buffer, "\n") != NULL)
            break;
    }
    printf("%s\n", strstr(buffer, "\n"));
    if (shutdown(sock_fd, SHUT_RDWR) == -1) {
        err_msg = "Ошибка закрытия сокета";
        exit_code = errno;
        goto err;
    }
    close(sock_fd);
    return EXIT_SUCCESS;
err:
    perror(err_msg);
    close(sock_fd);
    return exit_code;
}
