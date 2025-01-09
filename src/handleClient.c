#include "../include/handleClient.h"
#include <stdio.h>

void *handle_client(void *arg)
{
    client_info_t     *info        = (client_info_t *)arg;
    int                newsockfd   = info->socket_fd;
    struct sockaddr_in client_addr = info->client_addr;
    int                valread;
    int                valwrite;
    char               buffer[BUFFER_SIZE];
    char               method[BUFFER_SIZE];
    char               uri[BUFFER_SIZE];
    char               version[BUFFER_SIZE];
    const char         resp[] = "HTTP/1.0 200 OK\r\n"
                                "Server: webserver-c\r\n"
                                "Content-type: text/html\r\n\r\n"
                                "<html>hello, world</html>\r\n";

    // Read from the socket
    valread = (int)read(newsockfd, buffer, BUFFER_SIZE);
    if(valread < 0)
    {
        perror("webserver (read)");
        close(newsockfd);
        free(info);
        return NULL;
    }

    // Read the request
    sscanf(buffer, "%1023s %1023s %1023s", method, uri, version);

    // Log the request
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    // Respond with a simple HTML page
    valwrite = (int)write(newsockfd, resp, strlen(resp));
    if(valwrite < 0)
    {
        perror("webserver (write)");
    }

    // Close connection and free memory
    close(newsockfd);
    free(info);
    return NULL;
}
