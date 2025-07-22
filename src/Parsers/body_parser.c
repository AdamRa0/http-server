#include "../cJSON/cJSON.h"
#include "headers.h"
#include "http_req_parser.h"
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* supported_content_types[] = {
    "application/json",
    "appplication/x-www-form-urlencoded",
    "multipart/form-data",
};

void parse_body(char* request_body, HTTPParserResult* struct_parser)
{
    enum HTTPMethods method = struct_parser->method;

    switch(method)
    {
        case POST:
            // Check if req_body is of required content_type and content_length
            break;
        case PUT:
            // Check if req_body is of required content_type and content_length
            break;
        case PATCH:
            // Check if req_body is of required content_type and content_length
            break;
        default:
            break;
    }
}

ssize_t cast_char_to_ssize(char* value)
{
    errno = 0;

    int base = 10;
    char* p_value;

    long val = strtol(value, &p_value, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        perror("strtol");
        return -1;
    }

    if (p_value == value) {
        fprintf(stderr, "No digits were found\n");
        return -1;
    }

    return (ssize_t)val;
}

/*
Checks if request body is of requested type and appropriate length

Arguments
---------
request_body: data from request
body_type: Content-Type header value
body_length: Content-Length header value
*/
void body_checker(char* request_body, char* body_type, char* body_length)
{
    ssize_t req_body_len = strlen(request_body);
    ssize_t content_header_value = cast_char_to_ssize(body_length);
    
}

bool json_valid(char* data)
{
    cJSON* json = cJSON_Parse(data);

    return json == NULL;
}