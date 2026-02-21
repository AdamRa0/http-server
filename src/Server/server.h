#ifndef SERVER_H
#define SERVER_H

#define INITIAL_MESSAGE_SIZE 4096
#define MAX_MESSAGE_SIZE (10 * 1024 * 1024)

#include "../cJSON/cJSON.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>

int set_num_of_threads(cJSON* config_data);

int set_port_number(cJSON* config_data);

int set_nonblocking(int fd);

int setup_server_socket(int port);

void add_client_socket_to_event_loop(int e_fd, int sock_fd, struct epoll_event ev, struct sockaddr_in6 client_addr, socklen_t client_addr_len, struct timeval timeout);

void on_socket_available_to_read(int e_fd, int s_fd, cJSON* config_data, struct sockaddr_in6 client_addr, socklen_t client_addr_len, struct epoll_event ev);

void on_socket_available_to_write(void* ptr, int e_fd);

void run_event_loop(int e_fd, int s_fd, struct epoll_event ev, struct epoll_event* events, int num_events, struct timeval timeout, cJSON* config_data);

void close_server(int e_fd, int server_sock_fd, cJSON* config_data);
#endif