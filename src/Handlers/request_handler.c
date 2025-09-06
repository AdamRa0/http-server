#include "error_handler.h"
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
        case HEAD:
            const char* default_filename = "index.html";
            char* default_path = build_path(default_filename, RESPONSE_TYPE_OK);
            parse_headers(result->headers);
            set_server_response(result, OK_STATUS_CODE, OK_STATUS, RESPONSE_TYPE_OK_HEAD, default_path);
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

            // TODO: do something with URI (Implement routing)

            const char* filename_index = "index.html";
            char* index_path = build_path(filename_index, RESPONSE_TYPE_OK);
            parse_headers(result->headers);
            set_server_response(result, OK_STATUS_CODE, OK_STATUS, RESPONSE_TYPE_OK, index_path);
            break;
        case NONE:
            server_error_handler(result);
            break;
        default:
            break;
    }
}