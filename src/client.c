#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __APPLE__
    #define SOCK_CLOEXEC 0
#endif

#define BUFFER_SIZE 1024
#define TEN 10

int main(int argc, char *argv[])
{
    const char        *server_ip;
    long               server_port;
    const char        *uri;
    int                sockfd;
    struct sockaddr_in server_addr;
    char               send_buffer[BUFFER_SIZE];
    char               recv_buffer[BUFFER_SIZE];
    ssize_t            bytes_received;

    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <uri>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_ip   = argv[1];
    server_port = strtol(argv[2], NULL, TEN);
    uri         = argv[3];

    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);

    if(sockfd < 0)
    {
        perror("Failed to create socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons((uint16_t)server_port);

    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        close(sockfd);
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    snprintf(send_buffer, BUFFER_SIZE, "%s HTTP/1.1\r\nHost: %s\r\n\r\n", uri, server_ip);
    if(send(sockfd, send_buffer, strlen(send_buffer), 0) < 0)
    {
        perror("Failed to send");
        close(sockfd);
        return 1;
    }

    printf("URI sent successfully.\n");

    // Receive response from server and print it
    while((bytes_received = recv(sockfd, recv_buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        recv_buffer[bytes_received] = '\0';    // Null-terminate the received data
        printf("%s", recv_buffer);
    }

    if(bytes_received < 0)
    {
        perror("Failed to receive");
    }

    close(sockfd);
    return 0;
}
