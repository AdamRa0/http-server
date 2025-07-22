#include "stdio.h"
#include "body_parser.h"
#include "headers.h"
#include "http_req_parser.h"
#include "../DataStructures/hash_table.h"
#include <stdlib.h>
#include <string.h>

enum HTTPMethods method_comparor(char* method)
{
    if (strcmp(method, "GET") == 0)
    {
        return GET;
    }

    else if (strcmp(method, "POST") == 0)
    {
        return POST;
    }

    else if (strcmp(method, "PUT") == 0)
    {
        return PUT;
    }

    else if (strcmp(method, "PATCH") == 0)
    {
        return PATCH;
    }

    else if (strcmp(method, "DELETE") == 0)
    {
        return DELETE;
    }

    else if (strcmp(method, "OPTIONS") == 0)
    {
        return OPTIONS;
    }

    else if (strcmp(method, "TRACE") == 0)
    {
        return TRACE;
    }

    else if (strcmp(method, "CONNECT") == 0)
    {
        return CONNECT;
    }

    else if (strcmp(method, "HEAD") == 0)
    {
        return HEAD;
    }

    else
    {
        return NONE;
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

    char* crlf = strstr(data, "\r\n");

    if (crlf)
    {
        headers_start = crlf + 2;
    } else
    {
        char* lf = strstr(data, "\n");
        if (lf)
        {
            headers_start = lf + 1;
        }
        else 
        {
            free(data_dup);
            return result;
        }
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
            parse_headers(result.headers);
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
                parse_headers(result.headers);
            }

            *lf_x_2 = '\n';
        } else 
        {
            if (strlen(headers_start) > 0)
            {
                result.headers = strdup(headers_start);
                parse_headers(result.headers);
            }
        }
    }

    if (body_start && *body_start != '\0')
    {
        result.request_body = strdup(body_start);
        parse_body(result.request_body, &result);
    }

    // set connection status
    set_connection_status(&result);

    free(data_dup);
    return result;
}