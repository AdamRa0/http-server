#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "path_builder.h"
#include "cJSON/cJSON.h"
#include "Handlers/file_handler.h"
#include "Parsers/http_req_parser.h"

#define MAX_MESSAGE_SIZE 40960

volatile sig_atomic_t received_signal = 0;

void signal_handler(int signal_num)
{
    received_signal = signal_num;
}

int main()
{

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);

    char* conf_file_path = build_path("cerver.json", NULL, NULL, true);

    int port = 0;

    FileData conf_data = read_file(conf_file_path);

    cJSON* conf_json_data = cJSON_ParseWithLength(conf_data.file_content, strlen(conf_data.file_content));

    if (conf_json_data != NULL)
    {
        cJSON* servers = cJSON_GetObjectItemCaseSensitive(conf_json_data, "servers");

        cJSON* server = NULL;

        cJSON_ArrayForEach(server, servers)
        {
            cJSON* server_port = cJSON_GetObjectItemCaseSensitive(server, "port");

            if (!cJSON_IsNumber(server_port))
            {
                cJSON_Delete(conf_json_data);
                perror("Could not set server port value");
            }

            port = (int) server_port->valuedouble;
        }
    }

    struct sockaddr_in6 server_sockaddr_in;
    memset(&server_sockaddr_in, 0, sizeof(server_sockaddr_in));

    server_sockaddr_in.sin6_family = AF_INET6;
    server_sockaddr_in.sin6_addr = in6addr_any;
    server_sockaddr_in.sin6_port = htons(port);

    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);

    int optval = 1;

    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to set REUSEADDR option");
        close(socket_fd);
        return -1;
    }

    if(setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to set KEEPALIVE option");
        close(socket_fd);
        return -1;
    }

    optval = 0;

    if(setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to disable IPV6_V6ONLY option");
        close(socket_fd);
        return -1;
    }

    if (bind(socket_fd, (struct sockaddr*)&server_sockaddr_in, sizeof(server_sockaddr_in)) == 0) 
    {
        printf("Server running on port %d\n", port);
    } else if (port == 80 && errno == EACCES) 
    {
        printf("Port 80 requires root privileges, trying port 8080...\n");

        server_sockaddr_in.sin6_port = htons(8080);

        if (bind(socket_fd, (struct sockaddr*)&server_sockaddr_in, sizeof(server_sockaddr_in)) == 0) 
        {
            printf("Server running on port 8080\n");
            printf("Access at: http://localhost:8080\n");
        } else 
        {
            perror("Failed to bind socket");
            close(socket_fd);
            return -1;
        }
    }

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

        HTTPParserResult* result = (HTTPParserResult* ) malloc(sizeof(HTTPParserResult));
        result->config_data = conf_json_data;

        enum ConnectionStatus client_connection_status;

        if (received_signal == SIGTERM || received_signal == SIGINT || received_signal == SIGSEGV)
        {
            // Log error
            perror("Shutting server down.");
            break;
        }

        int accepted_conn = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (accepted_conn < 0) 
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("Accept failed");
            break;
        }

        char buffer[MAX_MESSAGE_SIZE] = {0};

        ssize_t read_bytes = recv(accepted_conn, buffer, sizeof(buffer) - 1, 0);

        if (read_bytes > 0)
        {
            buffer[read_bytes] = '\0';
            // TODO: Remove after validation and parsing features complete
            printf("Client message: %s\n", buffer);

            request_parser(buffer, result);
            client_connection_status = result->connection_status;
        } 
        
        else if (read_bytes == 0)
        {
            printf("Connection closed by client\n");
        }

        else
        {
            perror("Failed to receive message");
        }

        // TODO: Process request

        if (result->response_body)
        {
            write(accepted_conn, result->response_body, strlen(result->response_body));
        }
        
        if (client_connection_status != KEEP_ALIVE)
        {
            free(result);
            result = NULL;
            close(accepted_conn);
        }
    }

    cJSON_Delete(conf_json_data);
    close(socket_fd);
    return 0;
}