#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PORT        23
#define BUF_SIZE    16384
#define ANSWER      "Press control-C to interrupt any command."
#define IP          "64.13.139.230"
#define CMD_SIZE    1024
#define FIGLET      "figlet"
#define FORMAT      "%s /%s %s\n\r"


int main(int argc, char** argv)
{
    if(argc != 3){
        printf("Usage: %s <font> <message>\n", argv[0]);
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
    while (recv(sock_fd, buffer, BUF_SIZE - 1, 0) > 0)
		if (strstr(buffer, ANSWER) != NULL)
			break;

    char command[CMD_SIZE];
	if (snprintf(command, CMD_SIZE, FORMAT, FIGLET, argv[1], argv[2]) < 0) {
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
