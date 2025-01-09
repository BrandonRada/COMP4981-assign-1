#ifndef HANDLECLIENT
#define HANDLECLIENT

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct
{
    int                socket_fd;
    struct sockaddr_in client_addr;
} client_info_t;

void *handle_client(void *arg);

#endif    // HANDLECLIENT
