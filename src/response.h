#ifndef RESPONSE_H
#define RESPONSE_H

#include "Parsers/http_req_parser.h"

void set_server_response(HTTPParserResult* result, int status_code, const char* status);

#endif