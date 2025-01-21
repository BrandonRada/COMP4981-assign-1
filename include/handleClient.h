#ifndef HANDLECLIENT
#define HANDLECLIENT

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct
{
    int                socket_fd;    // cppcheck-suppress unusedStructMember
    struct sockaddr_in client_addr;
} client_info;

void *handle_client(void *arg);
int   send_file(int socket_fd, const char *file_path);

#endif    // HANDLECLIENT
