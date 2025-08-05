#include "constants.h"

#include "Parsers/http_req_parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define RESPONSE_SIZE 1024

void set_server_response(HTTPParserResult* result, int status_code, const char* status)
{
    char* response = (char* ) malloc(RESPONSE_SIZE);
    char response_body[256];
    char date_buffer[64];

    memset(response_body, 0, sizeof(response_body));

    time_t now = time(NULL);
    struct tm* gmt_time = gmtime(&now);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

    char* response_data = "<html><body><h1>%d: %s</h1></body></html>";

    snprintf(response_body, sizeof(response_body), response_data, status_code, status);

    size_t response_len = strlen(response_body);

    

    snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE, status_code, status, date_buffer, response_len, response_body);

    result->response_body = response;
}