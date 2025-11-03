#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "path_builder.h"
#include "cJSON/cJSON.h"
#include "Handlers/file_handler.h"
#include "Parsers/http_req_parser.h"

#define MAX_MESSAGE_SIZE 40960
#define MAX_EVENTS = 10

volatile sig_atomic_t received_signal = 0;

void signal_handler(int signal_num)
{
    received_signal = signal_num;
}

int set_nonblocking(int sock_fd)
{
    int flags = fcntl(sock_fd, F_GETFL, 0);

    if (flags == -1)
    {
        return -1;
    }

    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        return -1;
    }

    return 0;
}

int main()
{
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
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
        exit(EXIT_FAILURE);
    }
    
    if(setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to set KEEPALIVE option");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    
    optval = 0;
    
    if(setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval)) < 0)
    {
        perror("Failed to disable IPV6_V6ONLY option");
        close(socket_fd);
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }
    }
    
    if (set_nonblocking(socket_fd) == -1)
    {
        perror("Failed to set socket as nonblocking");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 10) < 0)
    {
        perror("Server failed to listen.");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1)
    {
        perror("Failed to create event loop");
        exit(EXIT_FAILURE);
    }

    struct epoll_events ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    
    epoll_registered = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    if (epoll_registered == -1)
    {
        perror("Failed to add socket to event loop");
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        no_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);;

        if (no_fds == -1)
        {
            perror("error in epoll_wait");
            exit(EXIT_FAILURE);
        }

        
        if (received_signal == SIGTERM || received_signal == SIGINT || received_signal == SIGSEGV)
        {
            perror("Shutting server down.");
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL);
            close(socket_fd);
        }
        
        for (int i = 0; i < no_fds, ++i)
        {
            if (events[i].data.fd == socket_fd)
            {
                while(1)
                {
                    int accepted_conn = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

                    if (accepted_conn < 0) 
                    {
                        if (errno == EINTR)
                        {
                            free(result);
                            result = NULL;
                            continue;
                        }
                        perror("Accept failed");
                        break;
                    }

                    if (setsockopt(accepted_conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
                    {
                        perror("setsockopt failed");
                    }

                    set_nonblocking(accepted_conn);

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = accepted_conn;

                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accepted_conn, &ev) == -1)
                    {
                        perror("Failed to add client socket to event loop");
                        close(accepted_conn);
                    }
                }       
            } else 
            {

            }
        }
    }

    // while(1)
    // {
    //     enum ConnectionStatus client_connection_status = KEEP_ALIVE;
        
    //     if (received_signal == SIGTERM || received_signal == SIGINT || received_signal == SIGSEGV)
    //     {
    //         perror("Shutting server down.");
    //         break;
    //     }

    //     struct sockaddr_in6 client_addr;
    //     socklen_t client_addr_len = sizeof(client_addr);

    //     HTTPParserResult* result = (HTTPParserResult* ) malloc(sizeof(HTTPParserResult));
    //     memset(result, 0, sizeof(HTTPParserResult));

    //     result->config_data = conf_json_data;

    //     int accepted_conn = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

    //     if (accepted_conn < 0) 
    //     {
    //         if (errno == EINTR)
    //         {
    //             free(result);
    //             result = NULL;
    //             continue;
    //         }
    //         perror("Accept failed");
    //         break;
    //     }

    //     if (setsockopt(accepted_conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    //     {
    //         perror("setsockopt failed");
    //     }

    //     getpeername(accepted_conn, (struct sockaddr*)&client_addr, &client_addr_len);

    //     char* client_ip = (char* ) malloc(INET6_ADDRSTRLEN);

    //     if (IN6_IS_ADDR_V4MAPPED(&(client_addr.sin6_addr)))
    //     {
    //         struct in_addr client_ipv4;
    //         memcpy(&(client_ipv4), client_addr.sin6_addr.s6_addr + 12, 4);
    //         inet_ntop(AF_INET, &(client_ipv4), client_ip, INET_ADDRSTRLEN);
    //     }
    //     else
    //     {
    //         inet_ntop(AF_INET6, &(client_addr.sin6_addr), client_ip, INET6_ADDRSTRLEN);
    //     }

    //     result->client_ip = client_ip;

    //     while (client_connection_status == KEEP_ALIVE)
    //     {
    //         char buffer[MAX_MESSAGE_SIZE] = {0};
    
    //         ssize_t read_bytes = recv(accepted_conn, buffer, sizeof(buffer) - 1, 0);
    
    //         if (read_bytes > 0)
    //         {
    //             buffer[read_bytes] = '\0';
    //             // TODO: Remove after validation and parsing features complete
    //             printf("Client message: %s\n", buffer);
    
    //             request_parser(buffer, result);
    //             client_connection_status = result->connection_status;
    //         } 
            
    //         else if (read_bytes == 0)
    //         {
    //             printf("Connection closed by client\n");
    //             break;
    //         }
    
    //         else
    //         {
    //             if (errno == EAGAIN || errno == EWOULDBLOCK)
    //             {
    //                 printf("Timeout waiting for request\n");
    //             }
    //             else
    //             {
    //                 perror("Failed to receive message\n");
    //             }
    //             break;
    //         }

    //         int cork = 1;

    //         setsockopt(accepted_conn, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));

    //         if (result->response_headers)
    //         {
    //             send(accepted_conn, result->response_headers, result->response_headers_size, 0);
    //             free(result->response_headers);
    //             result->response_headers = NULL;
    //         }
    
    //         if (result->data_content)
    //         {
    //             send(accepted_conn, result->data_content, result->response_size, 0);
    //             if(result->data_mime_type) 
    //             {
    //                 free(result->data_mime_type);
    //                 result->data_mime_type=NULL;
    //             }
    
    //             if(result->data_content) 
    //             {
    //                 free(result->data_content);
    //                 result->data_content=NULL;
    //             }
    //         }

    //         cork = 0;
    //         setsockopt(accepted_conn, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));

    //         if (result->URI) 
    //         {
    //             free(result->URI);
    //             result->URI = NULL;
    //         }

    //         if (result->headers) 
    //         {
    //             free(result->headers);
    //             result->headers = NULL;
    //         }

    //         if (result->request_body) 
    //         {
    //             free(result->request_body);
    //             result->request_body = NULL;
    //         }
            
    //         client_connection_status = result->connection_status;

    //         if (client_connection_status != KEEP_ALIVE)
    //         {
    //             printf("Closing connection...\n");
    //         }
    //     }
    
    //     if (result->client_ip) 
    //     {
    //         free(result->client_ip);
    //         result->client_ip = NULL;
    //     }

    //     if (result)
    //     {
    //         free(result);
    //         result = NULL;
    //     }

    //     close(accepted_conn);
    //     printf("Connection closed.\n");
    // }

    cJSON_Delete(conf_json_data);
    close(socket_fd);
    return 0;
}