#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Parsers/http_req_parser.h"

#define MAX_MESSAGE_SIZE 1024
#define PORT 7000

int main(int argc, char* argv[])
{
    struct sockaddr_in6 server_sockaddr_in;
    memset(&server_sockaddr_in, 0, sizeof(server_sockaddr_in));

    server_sockaddr_in.sin6_family = AF_INET6;
    server_sockaddr_in.sin6_addr = in6addr_any;
    server_sockaddr_in.sin6_port = htons(PORT);

    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);

    int optval = 0;

    if(setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to set IPV6_V6ONLY option");
        close(socket_fd);
        return -1;
    }

    if(bind(socket_fd, (struct sockaddr*)&server_sockaddr_in, sizeof(server_sockaddr_in)) < 0)
    {
        perror("Failed to bind socket");
        close(socket_fd);
        return -1;
    }

    printf("Server is running and listening on port 7000\n");

    if (listen(socket_fd, 10) < 0)
    {
        perror("Server failed to listen.");
        close(socket_fd);
        return -1;
    }

    while(1)
    {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        enum ConnectionStatus client_connection_status;

        int accepted_conn = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (accepted_conn < 0) 
        {
            perror("Failed to accept connection");
            continue;
        }

        char buffer[MAX_MESSAGE_SIZE] = {0};

        ssize_t read_bytes = recv(accepted_conn, buffer, sizeof(buffer) - 1, 0);

        if (read_bytes > 0)
        {
            buffer[read_bytes] = '\0';
            printf("Client message: %s\n", buffer);

            HTTPParserResult result = request_parser(buffer);
        } 
        
        else if (read_bytes == 0)
        {
            printf("Connection closed by client\n");
        }

        else
        {
            perror("Failed to receive message");
        }

        char response[] = "Hello from server";

        write(accepted_conn, response, strlen(response));
        
        close(accepted_conn);
    }

    close(socket_fd);

    return 0;
}