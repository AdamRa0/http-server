#include "HTTPReqParser.h"
#include <stdlib.h>
#include <string.h>

enum HTTPMethods method_comparor(char* method)
{
    if (strcmp(method, "GET") == 0)
    {
        return GET;
    }

    if (strcmp(method, "POST") == 0)
    {
        return POST;
    }

    if (strcmp(method, "PUT") == 0)
    {
        return PUT;
    }

    if (strcmp(method, "PATCH") == 0)
    {
        return PATCH;
    }

    if (strcmp(method, "DELETE") == 0)
    {
        return DELETE;
    }

    if (strcmp(method, "OPTIONS") == 0)
    {
        return OPTIONS;
    }

    if (strcmp(method, "TRACE") == 0)
    {
        return TRACE;
    }

    if (strcmp(method, "CONNECT") == 0)
    {
        return CONNECT;
    }

    if (strcmp(method, "HEAD") == 0)
    {
        return HEAD;
    }
}

HTTPParserResult request_parser(char* data) 
{
    HTTPParserResult result = {0};
    enum HTTPMethods method;

    if (data == NULL || strlen(data) == 0)
    {
        return result;
    }

    char* data_dup = strdup(data);
 
    int found_boundary = 0;

    for(int i = 0; i < strlen(data_dup) - 1; i++)
    {
        if (data_dup[i] == '\r' && data_dup[i + 1] == '\n' && 
            data_dup[i + 2] == '\r' && data_dup[i + 3] == '\n') {
            data_dup[i + 2] = '#';
            found_boundary = 1;
            break;
        }
        else if (data_dup[i] == '\n' && data_dup[i + 1] == '\n') {
            data_dup[i + 1] = '#';
            found_boundary = 1;
            break;
        }
    }

    char* request_line = strtok(data_dup, "\r\n");
    if (request_line == NULL) {
        free(data_dup);
        return result;
    }

    char* method_str = strtok(request_line, " ");
    if (method_str == NULL) {
        free(data_dup);
        return result;
    }
    
    result.method = method_comparor(method_str);

    char* uri = strtok(NULL, " ");
    if (uri != NULL) {
        result.URI = strdup(uri);
    }

    char* http_version = strtok(NULL, " ");
    if (http_version != NULL) {
        char* version_num = strchr(http_version, '/');
        if (version_num != NULL) {
            result.http_version = atof(version_num + 1);
        }
    }

    char* headers = strtok(NULL, "#");
    if (headers != NULL) {
        result.headers = strdup(headers);
    }

    char* body = strtok(NULL, "");
    if (body != NULL) {
        result.request_body = strdup(body);
    }

    free(data_dup);

    return result;
}