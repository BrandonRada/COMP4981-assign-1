#include "../include/handleClient.h"

#define BUFFER_SIZE 1024

int send_file(int socket_fd, const char *file_path)
{
    int         file_fd;
    struct stat stat_buf;
    char        buffer[BUFFER_SIZE];
    ssize_t     read_bytes;
    ssize_t     sent_bytes;

    // Open the file for reading
    file_fd = open(file_path, O_RDONLY | O_CLOEXEC);
    if(file_fd < 0)
    {
        perror("send_file (open)");
        return -1;
    }

    // Get the file size
    if(fstat(file_fd, &stat_buf) < 0)
    {
        perror("send_file (fstat)");
        close(file_fd);
        return -1;
    }

    // Read from the file and send to the socket in chunks
    while((read_bytes = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        off_t offset = 0;
        while(offset < read_bytes)
        {
            sent_bytes = send(socket_fd, buffer + offset, (size_t)(read_bytes - offset), 0);
            if(sent_bytes < 0)
            {
                perror("send_file (send)");
                close(file_fd);
                return -1;
            }
            offset += sent_bytes;
        }
    }

    // Error reading the file
    if(read_bytes < 0)
    {
        perror("send_file (read)");
        close(file_fd);
        return -1;
    }

    // Close the file
    close(file_fd);
    return 0;
}

void *handle_client(void *arg)
{
    client_info       *info        = (client_info *)arg;
    int                newsockfd   = info->socket_fd;
    struct sockaddr_in client_addr = info->client_addr;
    struct stat        stat_buf;
    int                file_fd;
    int                valread;
    char               buffer[BUFFER_SIZE];
    char               method[BUFFER_SIZE];
    char               uri[BUFFER_SIZE];
    char               version[BUFFER_SIZE];
    char               header[BUFFER_SIZE];
    char               filepath[BUFFER_SIZE] = ".";
    const char         not_found[]           = "HTTP/1.0 404 Not Found\r\n\r\n";
    const char         not_allowed[]         = "HTTP/1.0 405 Method Not Allowed\r\n\r\n";

    // Read from the socket
    valread = (int)read(newsockfd, buffer, BUFFER_SIZE);
    if(valread < 0)
    {
        perror("webserver (read)");
        close(newsockfd);
        free(info);
        printf("Read from the socket connection closed\n");
        return NULL;
    }

    // Read the request
    sscanf(buffer, "%1023s %1023s %1023s", method, uri, version);

    // Log the request
    printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

    // Check for supported methods
    if(strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0)
    {
        write(newsockfd, not_allowed, sizeof(not_allowed) - 1);
        close(newsockfd);
        free(info);
        printf("Check for supported methods connection closed\n");
        return NULL;
    }

    // File serving logic
    strncat(filepath, uri, BUFFER_SIZE - strlen(filepath) - 1);
    file_fd = open(filepath, O_RDONLY | O_CLOEXEC);
    if(file_fd < 0)
    {
        write(newsockfd, not_found, sizeof(not_found) - 1);
        close(newsockfd);
        free(info);
        printf("File serving logic connection closed\n");
        return NULL;
    }

    // Read the file and prepare response
    fstat(file_fd, &stat_buf);
    snprintf(header, BUFFER_SIZE, "HTTP/1.0 200 OK\r\nContent-Length: %ld\r\n\r\n", stat_buf.st_size);

    write(newsockfd, header, strlen(header));
    if(strcmp(method, "GET") == 0)
    {
        send_file(newsockfd, filepath);
    }

    // Close file descriptor and connection
    close(file_fd);
    close(newsockfd);
    free(info);
    printf("connection closed\n");
    return NULL;
}
