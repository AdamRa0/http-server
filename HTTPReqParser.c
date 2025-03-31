#include "HTTPReqParser.h"
#include <stdio.h>
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

    if (data == NULL || strlen(data) == 0)
    {
        return result;
    }

    char* data_dup = strdup(data);

    char* ptr1;

    char* request_line = strtok_r(data_dup, "\r\n", &ptr1);

    if (request_line == NULL)
    {
        request_line = strtok_r(data_dup, "\n", &ptr1);

        if (request_line == NULL)
        {
            free(data_dup);
            return result;
        }
    }

    char* req_line_ptr;

    char* method_str = strtok_r(request_line, " ", &req_line_ptr);

    if (method_str == NULL)
    {
        free(data_dup);
        return result;
    }

    result.method = method_comparor(method_str);

    char* uri = strtok_r(NULL, " ", &req_line_ptr);

    if (uri != NULL)
    {
        result.URI = strdup(uri);
    }

    char* http_version = strtok_r(NULL, " ", &req_line_ptr);

    if (http_version != NULL)
    {
        char* ver_num = strchr(http_version, '/');

        if (ver_num != NULL)
        {
            result.http_version = atof(ver_num + 1);
        }
    }

    char* headers_start = data;

    headers_start = strstr(headers_start, request_line);

    if (headers_start)
    {
        headers_start += strlen(request_line);

        printf("Headers Start: %s\n", headers_start);

        if (*headers_start == '\r' && *(headers_start + 1) == '\n')
        {
            headers_start += 2;
        } else if (*headers_start == '\n')
        {
            headers_start += 1;
        }

    } else
    {
        free(data_dup);
        return result;
    }

    char* body_start = NULL;

    char* crlf_x_2 = strstr(headers_start, "\r\n\r\n");

    if (crlf_x_2)
    {
        body_start = crlf_x_2 + 4;

        *crlf_x_2 = '\0';

        if (strlen(headers_start) > 0)
        {
            result.headers = strdup(headers_start);
        }

        *crlf_x_2 = '\r';
    } else 
    {
        char* lf_x_2 = strstr(headers_start, "\n\n");

        if (lf_x_2)
        {
            body_start = lf_x_2 + 2;

            *lf_x_2 = '\0';

            if (strlen(headers_start) > 0)
            {
                result.headers = strdup(headers_start);
            }

            *lf_x_2 = '\n';
        } else 
        {
            if (strlen(headers_start) > 0)
            {
                result.headers = strdup(headers_start);
            }
        }
    }

    if (body_start && *body_start != '\0')
    {
        result.request_body = strdup(body_start);
    }

    free(data_dup);
    return result;
}