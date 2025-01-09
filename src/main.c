#include "../include/handleClient.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(void)
{
    struct sockaddr_in host_addr;
    struct sockaddr_in client_addr;
    socklen_t          client_len = sizeof(client_addr);
    int                sockfd;
    pthread_t          tid;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");

    // Create the address to bind the socket to
    host_addr.sin_family      = AF_INET;
    host_addr.sin_port        = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) != 0)
    {
        perror("webserver (bind)");
        close(sockfd);
        return 1;
    }
    printf("socket successfully bound to address\n");

    // Listen for incoming connections
    if(listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        close(sockfd);
        return 1;
    }
    printf("server listening for connections...\n");

    for(;;)
    {
        // Accept incoming connections
        client_info_t *info = malloc(sizeof(client_info_t));
        if(!info)
        {
            perror("webserver (malloc)");
            continue;
        }

        info->socket_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if(info->socket_fd < 0)
        {
            perror("webserver (accept)");
            free(info);
            continue;
        }

        info->client_addr = client_addr;
        printf("connection accepted\n");

        // Create a new thread to handle the client
        if(pthread_create(&tid, NULL, handle_client, info) != 0)
        {
            perror("webserver (pthread_create)");
            close(info->socket_fd);
            free(info);
            continue;
        }

        // Detach the thread so its resources are released when it finishes
        pthread_detach(tid);
    }
}
