#include "response_handler.h"
#include "../path_builder.h"
#include "../constants.h"
#include "../Parsers/body_parser.h"
#include "../Parsers/headers.h"
#include "../Parsers/http_req_parser.h"

void handle_request(HTTPParserResult* result)
{
    // Method handling
    switch(result->method)
    {
        case PATCH:
            if (!result->request_body)
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case PUT:
            if (!result->request_body)
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case POST:
            if (!result->request_body)
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
                break;
            }
            parse_headers(result->headers);
            set_connection_status(result);
            parse_body(result->request_body, result);
            break;
        case DELETE:
            if (result->request_body)
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
            }
            parse_headers(result->headers);
            set_connection_status(result);
            break;
        case GET:

            if (result->request_body)
            {
                const char* filename = "400.html";
                char* path = build_path(filename, RESPONSE_TYPE_ERROR);

                set_server_response(result, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
                break;
            }

            char* uri = result->URI;

            // TODO: do something with URI (Implement routing)

            const char* filename_index = "index.html";
            char* index_path = build_path(filename_index, RESPONSE_TYPE_OK);
            parse_headers(result->headers);
            set_server_response(result, OK_STATUS_CODE, OK_STATUS, RESPONSE_TYPE_OK, index_path);
            break;
        case NONE:
            const char* filename_internal_error = "500.html";
            char* internal_error_path = build_path(filename_internal_error, RESPONSE_TYPE_ERROR);

            set_server_response(result, INTERNAL_SERVER_ERROR_STATUS_CODE, INTERNAL_SERVER_ERROR_STATUS, RESPONSE_TYPE_ERROR, internal_error_path);
            break;
        default:
            break;
    }
}