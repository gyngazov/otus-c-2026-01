#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT    23
#define IP      "64.13.139.230"
#define BUF_LEN 65536

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char *hello = "figlet /basic 123abc";
    char buffer[BUF_LEN] = {0};

    // 1. Create socket (IPv4, TCP)
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT); // Convert port to network byte order

    // 2. Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // 3. Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // 4. Send and receive data
    send(sockfd, hello, strlen(hello), 0);
    printf("Message sent\n");
    recv(sockfd, &buffer, BUF_LEN, 0);
    //read(sockfd, buffer, BUF_LEN);
    printf("Server says: %s\n", buffer);

    // 5. Close the connection
    close(sockfd);
    return 0;
}
