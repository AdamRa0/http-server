#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H

#include "../cJSON/cJSON.h"

enum HTTPMethods {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
    TRACE,
    CONNECT,
    HEAD,
    NONE
};

enum ConnectionStatus {
    KEEP_ALIVE,
    CLOSE,
};

typedef struct HTTPParserResult{
    enum HTTPMethods method;
    enum ConnectionStatus connection_status;
    float http_version;
    char* URI;
    char* headers;
    char* request_body;
    char* response_body;
    char* data_mime_type;
    char* data_content;
    cJSON* config_data;
    cJSON* web_page_root;
    cJSON* error_page_root;
    cJSON* default_index_file;
} HTTPParserResult;

void request_parser(char* data, HTTPParserResult* result);

#endif