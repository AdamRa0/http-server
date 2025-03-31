#ifndef __HTTPREQPARSER_H
#define ___HTTPREQPARSER_H

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
    char* headers; // TODO: Convert this to dictionary
    char* request_body; // TODO: Also convert this to dictionary
} HTTPParserResult;

HTTPParserResult request_parser(char* data);

#endif