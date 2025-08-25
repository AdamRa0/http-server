#include "body_parser.h"
#include "headers.h"
#include "http_req_parser.h"

#include "../constants.h"
#include "../DataStructures/hash_table.h"
#include "../file_ops.h"
#include "../path_builder.h"
#include "../Handlers/response_handler.h"

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

void request_parser(char* data, HTTPParserResult* result) 
{
    if (data == NULL || strlen(data) == 0)
    {
        const char* filename = "400.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
        return;
    }

    char* data_dup = strdup(data);

    char* ptr1;

    char* request_line = strtok_r(data_dup, "\r\n", &ptr1);

    if (request_line == NULL)
    {
        request_line = strtok_r(data_dup, "\n", &ptr1);

        if (request_line == NULL)
        {
            const char* filename = "400.html";
            char* path = build_path(filename, RESPONSE_TYPE_ERROR);

            set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);

            free(data_dup);
            data_dup = NULL;
            return;
        }
    }

    char* req_line_ptr;

    char* method_str = strtok_r(request_line, " ", &req_line_ptr);

    if (method_str == NULL)
    {
        const char* filename = "400.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);

        free(data_dup);
        data_dup = NULL;
        return;
    }

    result->method = method_comparor(method_str);

    if (result->method == NONE)
    {
        const char* filename = "500.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, INTERNAL_SERVER_ERROR_STATUS_CODE, INTERNAL_SERVER_ERROR_STATUS, RESPONSE_TYPE_ERROR, path);

        free(data_dup);
        data_dup = NULL;
        return;
    }

    char* uri = strtok_r(NULL, " ", &req_line_ptr);

    if (uri != NULL)
    {
        result->URI = strdup(uri);
    } else 
    {
        const char* filename = "400.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
        free(data_dup);
        data_dup = NULL;
        return;    
    }

    char* http_version = strtok_r(NULL, " ", &req_line_ptr);

    if (http_version != NULL)
    {
        char* ver_num = strchr(http_version, '/');

        if (ver_num != NULL)
        {
            result->http_version = atof(ver_num + 1);
        } else 
        {
            const char* filename = "400.html";
            char* path = build_path(filename, RESPONSE_TYPE_ERROR);

            set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
            free(data_dup);
            data_dup = NULL;
            return;
        }
    } else 
    {
        const char* filename = "400.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
        free(data_dup);
        data_dup = NULL;
        return;
    }

    // If method == GET, set current dummy response
    if (result->method == GET)
    {

        char* uri = result->URI;

        const char* filename = "index.html";
        char* path = build_path(filename, RESPONSE_TYPE_OK);

        set_server_response(result, OK_STATUS_CODE, OK_STATUS, RESPONSE_TYPE_OK, path);
        free(data_dup);
        data_dup = NULL;
        return;
    }

    // Http version not 1.1
    if (result->http_version != 1.1f)
    {
        const char* filename = "500.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(result, INTERNAL_SERVER_ERROR_STATUS_CODE, INTERNAL_SERVER_ERROR_STATUS, RESPONSE_TYPE_ERROR, path);
        free(data_dup);
        data_dup = NULL;
        return;
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
            const char* filename = "400.html";
            char* path = build_path(filename, RESPONSE_TYPE_ERROR);

            set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
            free(data_dup);
            data_dup = NULL;
            return;
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

            if (!(strstr(headers_start, "Host:")))
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
                free(data_dup);
                data_dup = NULL;
                return;
            }

            result->headers = strdup(headers_start);
            parse_headers(result->headers);
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
                result->headers = strdup(headers_start);
                parse_headers(result->headers);
            }

            *lf_x_2 = '\n';
        } else 
        {
            if (strlen(headers_start) > 0)
            {
                result->headers = strdup(headers_start);
                parse_headers(result->headers);
            }
        }
    }

    if (body_start && *body_start != '\0')
    {
        result->request_body = strdup(body_start);
        // Method will be part of request processor
        parse_body(result->request_body, result);
    }

    // set connection status
    set_connection_status(result);

    free(data_dup);
    data_dup = NULL;
}