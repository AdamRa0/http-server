#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H

enum HTTPMethods {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
    TRACE,
    CONNECT,
    HEAD
};

typedef struct HTTPParserResult{
    enum HTTPMethods method;
    char* URI;
    float http_version;
    char* headers;
    char* request_body;
} HTTPParserResult;

HTTPParserResult request_parser(char* data);

void parse_headers(char* headers);

#endif