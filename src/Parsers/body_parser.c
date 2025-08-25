#include "../cJSON/cJSON.h"
#include "../constants.h"
#include "../path_builder.h"
#include "../Handlers/response_handler.h"

#include "body_parser.h"
#include "headers.h"
#include "http_req_parser.h"

#include "Validators/validators.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

char* supported_content_types[] = {
    "application/json",
    "application/x-www-form-urlencoded",
    "multipart/form-data",
};

parsing_functions functions[] = {
    json_valid,
    url_encoded_form_valid,
    multipart_form_data_valid
};

/**
@brief checks if request body is of requested type and appropriate length
@return null

@param request_body data from request
@param body_type Content-Type header value
@param body_length Content-Length header value
*/
void body_checker(char* request_body, char* body_type, char* body_length, HTTPParserResult* parser_struct)
{
    bool is_valid = false;

    if (body_type != NULL && body_length != NULL && request_body != NULL)
    {
        ssize_t req_body_len = strlen(request_body);
        ssize_t content_header_value = cast_char_to_ssize(body_length);

        ssize_t array_size = sizeof(supported_content_types) / sizeof(supported_content_types[0]);
        
        if (req_body_len == content_header_value)
        {
            for (int i = 0; i < array_size; i++)
            {
                if ((strcmp(body_type, supported_content_types[i]) == 0))
                {
                    is_valid = functions[i](request_body);
                }
            }
        }
    }

    if (!is_valid)
    {
        const char* filename = "400.html";
        char* path = build_path(filename, RESPONSE_TYPE_ERROR);

        set_server_response(parser_struct, BAD_REQUEST_STATUS_CODE, BAD_REQUEST_STATUS, RESPONSE_TYPE_ERROR, path);
    } else 
    {
        const char* filename = "index.html";
        char* path = build_path(filename, RESPONSE_TYPE_OK);
    
        set_server_response(parser_struct, OK_STATUS_CODE, OK_STATUS, RESPONSE_TYPE_OK, path);
    }
}

void parse_body(char* request_body, HTTPParserResult* struct_parser)
{
    enum HTTPMethods method = struct_parser->method;
    char* content_type = get_header_value(CONTENT_TYPE_HEADER_NAME);
    char* content_length = get_header_value(CONTENT_LENGTH_HEADER_NAME);

    if (method == POST || method == PUT || method == PATCH)
    {
        body_checker(request_body, content_type, content_length, struct_parser);
    }
}