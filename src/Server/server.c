#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "server.h"

#include "../cJSON/cJSON.h"
#include "../Handlers/error_handler.h"
#include "../Parsers/http_req_parser.h"
#include "../Worker/thread_pool.h"

ThreadPool* worker_pool = NULL;

int set_num_of_threads(cJSON* config_data)
{
    if (config_data != NULL)
    {
        cJSON* num_of_threads = cJSON_GetObjectItemCaseSensitive(config_data, "num_of_threads");
        
        if (!cJSON_IsNumber(num_of_threads) || cJSON_IsNull(num_of_threads))
        {
            cJSON_Delete(config_data);
            perror("Could not set number of threads value");
            return 0;
        }

        return (int) num_of_threads->valuedouble;
    }

    return 0;
}

int set_port_number(cJSON* config_data)
{
    if (config_data != NULL)
    {
        cJSON* servers = cJSON_GetObjectItemCaseSensitive(config_data, "servers");
        
        cJSON* server = NULL;
        
        cJSON_ArrayForEach(server, servers)
        {
            cJSON* server_port = cJSON_GetObjectItemCaseSensitive(server, "port");
            
            if (!cJSON_IsNumber(server_port) || cJSON_IsNull(server_port))
            {
                cJSON_Delete(config_data);
                perror("Could not set server port value");
                return 0;
            }
            
            return (int) server_port->valuedouble;
        }
    }

    return 0;
}

int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
    {
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        return -1;
    }

    return 0;
}

int setup_server_socket(int port)
{
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
    } else if (errno == EACCES) 
    {
        perror("Failed to bind socket");
        close(socket_fd);
        exit(EXIT_FAILURE);
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

    return socket_fd;
}

void add_client_socket_to_event_loop(int e_fd, int sock_fd, struct epoll_event ev, 
                                        struct sockaddr_in6 client_addr, 
                                        socklen_t client_addr_len, 
                                        struct timeval timeout)
{
    while(1)
    {
        int accepted_conn = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (accepted_conn < 0) 
        {
            if (errno == EINTR)
            {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
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

        if (epoll_ctl(e_fd, EPOLL_CTL_ADD, accepted_conn, &ev) == -1)
        {
            perror("Failed to add client socket to event loop");
            close(accepted_conn);
        }
    }       
}

void on_socket_available_to_read(int e_fd, int s_fd, cJSON* config_data, 
                                    struct sockaddr_in6 client_addr, 
                                    socklen_t client_addr_len, 
                                    struct epoll_event ev)
{
    HTTPParserResult* result = (HTTPParserResult*)malloc(sizeof(HTTPParserResult));
    memset(result, 0, sizeof(HTTPParserResult));

    result->config_data = config_data;
    result->client_socket_fd = s_fd;
    result->connection_status = KEEP_ALIVE;

    getpeername(s_fd, (struct sockaddr*)&client_addr, &client_addr_len);

    char* client_ip = (char*)malloc(INET6_ADDRSTRLEN);

    if (IN6_IS_ADDR_V4MAPPED(&(client_addr.sin6_addr)))
    {
        struct in_addr client_ipv4;
        memcpy(&(client_ipv4), client_addr.sin6_addr.s6_addr + 12, 4);
        inet_ntop(AF_INET, &(client_ipv4), client_ip, INET_ADDRSTRLEN);
    }
    else
    {
        inet_ntop(AF_INET6, &(client_addr.sin6_addr), client_ip, INET6_ADDRSTRLEN);
    }

    result->client_ip = client_ip;

    char* buffer = (char*)malloc(INITIAL_MESSAGE_SIZE);
    memset(buffer, 0, INITIAL_MESSAGE_SIZE);

    size_t buffer_size = INITIAL_MESSAGE_SIZE;
    size_t total_read = 0;

    while (total_read < MAX_MESSAGE_SIZE)
    {
        ssize_t read_bytes = recv(s_fd, buffer + total_read, 
                                   buffer_size - total_read - 1, MSG_DONTWAIT);

        if (read_bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            else
            {
                perror("Failed to receive message");
                free(buffer);
                free(result->client_ip);
                free(result);
                close(s_fd);
                return;
            }
        }
        else if (read_bytes == 0)
        {
            close(s_fd);
            printf("Connection closed by client\n");
            free(buffer);
            free(result->client_ip);
            free(result);
            return;
        }

        total_read += read_bytes;

        if (strstr(buffer, "\r\n\r\n"))
        {
            break;
        }

        if (total_read >= buffer_size - 1)
        {
            buffer_size *= 2;
            if (buffer_size > MAX_MESSAGE_SIZE)
            {
                // Write payload too large html page to http parser result struct
                payload_too_large_handler(result);

                perror("Payload received too large");

                free(buffer);
                free(result->client_ip);
                free(result);

                // set client socket to write
                ev.events = EPOLLOUT | EPOLLET;
                ev.data.ptr = result;

                if(epoll_ctl(e_fd, EPOLL_CTL_MOD, s_fd, &ev) == -1)
                {
                    perror("Failed to monitor epoll for writing");
                } 
                return;
            }
            buffer = realloc(buffer, buffer_size);
            if (!buffer)
            {
                perror("Failed to reallocate buffer");
                free(result->client_ip);
                free(result);
                close(s_fd);
                return;
            }
        }
    }

    if (total_read == 0)
    {
        free(buffer);
        free(result->client_ip);
        free(result);
        return;
    }

    buffer[total_read] = '\0';

    ThreadJob* job = (ThreadJob*)malloc(sizeof(ThreadJob));
    job->worker = request_parser;
    job->buffer = buffer;
    job->result = result;
    job->epoll_fd = e_fd;
    job->socket_fd = s_fd;

    add_job_to_work_queue(worker_pool, job);
}

void on_socket_available_to_write(void* ptr, int s_fd)
{
    HTTPParserResult* result = (HTTPParserResult* ) ptr;
                    
    int client_conn_fd = result->client_socket_fd;
    
    int cork = 1;
    
    setsockopt(client_conn_fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));
    
    if (result->response_headers)
    {
        ssize_t sent_bytes = send(client_conn_fd, result->response_headers, result->response_headers_size, 0);
        
        if (sent_bytes < 0)
        {
            perror("Could not send headers.");
        }
        
        free(result->response_headers);
        result->response_headers = NULL;
    }

    if (result->data_content)
    {
        ssize_t sent_content = send(client_conn_fd, result->data_content, result->response_size, 0);
        
        if(sent_content < 0)
        {
            perror("Could not send content");
        }
        
        if(result->data_mime_type) 
        {
            free(result->data_mime_type);
            result->data_mime_type=NULL;
        }
        
        if(result->data_content) 
        {
            free(result->data_content);
            result->data_content=NULL;
        }
    }
    
    cork = 0;
    setsockopt(client_conn_fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(cork));

    enum ConnectionStatus client_connection_status = result->connection_status;
    
    if (result->URI) 
    {
        free(result->URI);
        result->URI = NULL;
    }

    if (result->headers) 
    {
        free(result->headers);
        result->headers = NULL;
    }

    if (result->request_body) 
    {
        free(result->request_body);
        result->request_body = NULL;
    }
    
    if (result->client_ip)
    {
        free(result->client_ip);
        result->client_ip = NULL;
    }

    if (s_fd == client_conn_fd && result)
    {
        free(result);
        result = NULL;
    }

    if (client_connection_status != KEEP_ALIVE)
    {
        close(client_conn_fd);
        return;
    }
}

void run_event_loop(int e_fd, int s_fd, struct epoll_event ev, 
                        struct epoll_event* events, 
                        int num_of_events, 
                        struct timeval timeout, 
                        cJSON* config_data)
{
    int num_of_threads = set_num_of_threads(config_data);

    worker_pool = init_thread_pool(num_of_threads);
    
    for(;;)
    {
        int no_fds = epoll_wait(e_fd, events, num_of_events, -1);

        if (no_fds == -1)
        {
            perror("error in epoll_wait");
            break;
        }
        
        for (int i = 0; i < no_fds; i++)
        {
            struct sockaddr_in6 client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            if (events[i].data.fd == s_fd)
            {
                add_client_socket_to_event_loop(e_fd, s_fd, ev, client_addr, client_addr_len, timeout);
            } else 
            {
                int conn = events[i].data.fd;

                if(events[i].events & EPOLLIN)
                {
                    on_socket_available_to_read(e_fd, conn, config_data, client_addr, client_addr_len, ev);
                } else if (events[i].events & EPOLLOUT)
                {
                    on_socket_available_to_write(events[i].data.ptr, conn);
                } else if (events[i].events & EPOLLERR)
                {
                    perror("Socket error occured");
                    close(conn);
                }
            }
        }
    }
}

void close_server(int e_fd, int server_sock_fd, cJSON* config_data)
{
    destroy_thread_pool(worker_pool);
    epoll_ctl(e_fd, EPOLL_CTL_DEL, server_sock_fd, NULL);
    close(e_fd);

    cJSON_Delete(config_data);
    close(server_sock_fd);
}