#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include "path_builder.h"
#include "cJSON/cJSON.h"
#include "Handlers/file_handler.h"
#include "Parsers/http_req_parser.h"
#include "Server/server.h"

#define MAX_EVENTS 10

volatile sig_atomic_t received_signal = 0;

void signal_handler(int signal_num)
{
    received_signal = signal_num;
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
    
    FileData conf_data = read_file(conf_file_path);

    free(conf_file_path);
    
    cJSON* conf_json_data = cJSON_ParseWithLength(conf_data.file_content, strlen(conf_data.file_content));
    
    int port = set_port_number(conf_json_data);
    
    int server_socket = setup_server_socket(port);

    int epoll_fd = epoll_create1(0);
    
    if (epoll_fd == -1)
    {
        perror("Failed to create event loop");
        exit(EXIT_FAILURE);
    }
    
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_socket;
    
    int epoll_registered = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &ev);

    if (epoll_registered == -1)
    {
        perror("Failed to add socket to event loop");
        exit(EXIT_FAILURE);
    }

    while(received_signal == 0)
    {
        run_event_loop(epoll_fd, server_socket, ev, events, MAX_EVENTS, timeout, conf_json_data);
    }

    if (received_signal == SIGINT || received_signal == SIGSEGV || received_signal == SIGTERM)
    {
        close_server(epoll_fd, server_socket, conf_json_data);
    }

    return 0;
}