#include "error_handler.h"
#include "response_handler.h"

#include "../constants.h"
#include "../path_builder.h"
#include "../Parsers/http_req_parser.h"

void bad_request_handler(HTTPParserResult* result)
{
    const char* filename = "400.html";
    char* path = build_path(filename, true, false);

    set_server_response(result, path);
}

void server_error_handler(HTTPParserResult* result)
{
    const char* filename = "500.html";
    char* path = build_path(filename, true, false);

    set_server_response(result, path);
}