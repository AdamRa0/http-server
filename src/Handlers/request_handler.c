#include "error_handler.h"
#include "response_handler.h"

#include "../path_builder.h"
#include "../constants.h"
#include "../Parsers/body_parser.h"
#include "../Parsers/headers.h"
#include "../Parsers/http_req_parser.h"

#include <string.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

void handle_request(HTTPParserResult* result)
{
    // Method handling
    switch(result->method)
    {
        case HEAD:
            const char* default_filename = "index.html";
            char* default_path = build_path(default_filename, false);
            parse_headers(result->headers);
            set_server_response(result, default_path);
            break;
        case OPTIONS:
            parse_headers(result->headers);
            set_server_response(result, NULL);
            break;
        case PATCH:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case PUT:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case POST:
            if (!result->request_body)
            {
                bad_request_handler(result);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case DELETE:
            if (result->request_body)
            {
                bad_request_handler(result);
            }
            parse_headers(result->headers);
            set_connection_status(result);
            break;
        case GET:

            if (result->request_body)
            {
                bad_request_handler(result);
                break;
            }

            char* uri = result->URI;

            const char* filename = (strcmp(result->URI, "/") == 0) ? "index.html" : result->URI + 1;

            char* file_path = build_path(filename, false);
            parse_headers(result->headers);
            set_server_response(result, file_path);
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