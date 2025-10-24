#include "error_handler.h"
#include "response_handler.h"

#include "../path_builder.h"
#include "../constants.h"
#include "../cJSON/cJSON.h"
#include "../DataStructures/hash_table.h"
#include "../Parsers/body_parser.h"
#include "../Parsers/headers.h"
#include "../Parsers/http_req_parser.h"

#include <stdio.h>
#include <string.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

void handle_request(HTTPParserResult* result, HashTable* h_dict)
{
    parse_headers(result->headers, h_dict);

    const char* host = get_header_value("Host", h_dict);

    if (host == NULL)
    {
        result->error_message = "Invalid host header value.";
        server_error_handler(result);
        return;
    }

    cJSON* appropriate_server_block = NULL;
    cJSON* server;

    cJSON* servers = cJSON_GetObjectItemCaseSensitive(result->config_data, "servers");

    cJSON_ArrayForEach(server, servers)
    {
        cJSON* server_name = cJSON_GetObjectItemCaseSensitive(server, "server_name");

        if(strstr(host, server_name->valuestring))
        {
            appropriate_server_block = server;
            break;
        }
    }

    cJSON* application_block = cJSON_GetObjectItemCaseSensitive(appropriate_server_block, "location");

    if (cJSON_IsNull(application_block))
    {
        result->error_message = "Could not resolve application block in configuration.";
        server_error_handler(result);
        return;       
    }

    cJSON* application_uri_path = cJSON_GetObjectItemCaseSensitive(application_block, "path");

    if (cJSON_IsNull(application_uri_path))
    {
        result->error_message = "Invalid home path.";
        server_error_handler(result);
        return;        
    }

    if (strncmp(result->URI, application_uri_path->valuestring, strlen(application_uri_path->valuestring)) == 0)
    {
        result->web_page_root = cJSON_GetObjectItemCaseSensitive(application_block, "root");
        result->default_index_file = cJSON_GetObjectItemCaseSensitive(application_block, "index");
        result->error_page_root = cJSON_GetObjectItemCaseSensitive(application_block, "error_pages");
    }

    switch(result->method)
    {
        case HEAD:
            char* default_path = build_path(result->default_index_file->valuestring, result->web_page_root->valuestring, NULL, false);
            set_server_response(result, default_path, h_dict);
            break;
        case OPTIONS:
            set_server_response(result, NULL, h_dict);
            break;
        case PATCH:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            set_connection_status(result, h_dict);
            parse_body(result->request_body, result, h_dict);
            break;
        case PUT:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            set_connection_status(result, h_dict);
            parse_body(result->request_body, result, h_dict);
            break;
        case POST:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            set_connection_status(result, h_dict);
            parse_body(result->request_body, result, h_dict);
            break;
        case DELETE:
            if (result->request_body)
            {
                bad_request_handler(result);
            }
            set_connection_status(result, h_dict);
            break;
        case GET:

            if (result->request_body)
            {
                bad_request_handler(result);
                break;
            }

            char* uri = result->URI ? result->URI : "/";

            const char* filename;
            
            if (strcmp(uri, "/") == 0 || uri[0] == '\0') 
            {
                filename = result->default_index_file->valuestring;
            } else if (uri[0] == '/') 
            {
                filename = uri + 1;
            } else 
            {
                filename = uri;
            }

            char* file_path = build_path(filename, result->web_page_root->valuestring, NULL, false);

            if (!is_path_safe(file_path, result->web_page_root->valuestring))
            {
                result->error_message = "Forbidden from accessing this path";

                forbidden_request_handler(result);
            }

            parse_headers(result->headers, h_dict);
            set_connection_status(result, h_dict);
            set_server_response(result, file_path, h_dict);
            break;
        case TRACE:
            server_error_handler(result);
            break;
        case CONNECT:
            server_error_handler(result);
            break;
        case NONE:
            server_error_handler(result);
            break;
        default:
            break;
    }
}