#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "../Parsers/http_req_parser.h"

void bad_request_handler(HTTPParserResult* result);

void forbidden_request_handler(HTTPParserResult* result);

void payload_too_large_handler(HTTPParserResult* result);

void server_error_handler(HTTPParserResult* result);

#endif