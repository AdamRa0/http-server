#include "../cJSON/cJSON.h"

#include "body_parser.h"
#include "../constants.h"
#include "headers.h"
#include "http_req_parser.h"

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

bool json_valid(char* data)
{
    cJSON* json = cJSON_Parse(data);

    return json != NULL;
}

bool multipart_form_data_valid(char* data)
{   
    char* boundary = get_header_value(BOUNDARY_HEADER_PARAM);

    const size_t MIN_BOUNDARY_LEN = 1;
    const size_t MAX_BOUNDARY_LEN = 70;

    static bool is_start_boundary = true;
    static bool is_middle_boundary = false;
    static bool is_end_boundary = false;
    
    // If boundary was not stored while parsing header
    // malformed request hence not valid
    if (boundary == NULL)
    {
        return false;
    }

    size_t boundary_len = strlen(boundary);
    
    // Validate boundary length
    if (boundary_len < MIN_BOUNDARY_LEN || boundary_len > MAX_BOUNDARY_LEN)
    {
        return false;
    }

    if (isspace(boundary[boundary_len - 1])) 
    {
        return false;
    }

    // is boundary RFC compliant
    for (size_t i = 0; i < boundary_len; i++) {
        unsigned char c = (unsigned char)boundary[i];
        if (c < 32 || c == 127) { 
            return false;
        }
    }

    // Validate request data
    char* p_line;
    char* crlf = "\r\n";
    char* lf = "\n";


    char* data_dup = strdup(data);

    char* line = strtok_r(data_dup, "\r\n", &p_line);

    if (line == NULL)
    {
        line = strtok_r(data_dup, "\n", &p_line);

        if (line == NULL)
        {
            free(data_dup);
            data_dup = NULL;

            return false;
        }
    }

    while (line != NULL)
    {
        char* boundary_pos = strstr(line, boundary);

        // Check if boundary exists
        if (boundary_pos != NULL)
        {
            // Check for valid boundary
            if (strcmp(boundary_pos - 1,  "-") == 0 && strcmp(boundary_pos - 2, "-") == 0)
            {
                // if end boundary, break out of while loop
                if (strcmp(boundary_pos + boundary_len + 1, "-") == 0 && strcmp(boundary_pos + boundary_len + 2, "-") == 0)
                {
                    is_middle_boundary = false;
                    is_end_boundary = true;
                    break;
                }

                // if not in start boundary, we are in middle boundary
                if (is_start_boundary == false)
                {
                    is_middle_boundary = true;
                }
            } else
            {
                // Invalid boundary hence malformed request
                return false;
            }

        }

        if (boundary_pos == NULL)
        {
            // Check if we are at beginning of data
            if (is_start_boundary) 
            {
                // we were expecting boundary at beginning of data
                // malformed request hence not valid
                return false;
            }
            else 
            {
                if (is_start_boundary)
                {
                    is_start_boundary = false;
                }

                // Check if Content-Disposition Header present
                char* content_disposition_pos = strstr(line, CONTENT_DISPOSITION_HEADER_NAME);

                if (content_disposition_pos == NULL)
                {
                    return false;
                }
            }
        }

        line = strtok_r(NULL, crlf, &p_line);

        if (line == NULL)
        {
            line = strtok_r(NULL, lf, &p_line);
        }
    }

    free(data_dup);
    data_dup = NULL;

    return true;
}

bool is_valid_form_char(char c) {
    return isalnum(c) || 
           c == '%' || c == '+' || c == '=' || c == '&' || 
           c == '-' || c == '_' || c == '.' || c == '~' ||
           c == ':' || c == '/' || c == '?' || c == '#' ||
           c == '[' || c == ']' || c == '@' || c == '!' ||
           c == '$' || c == '\'' || c == '(' || c == ')' ||
           c == '*' || c == ',' || c == ';';
}

bool is_valid_percent_encoding(const char* data, size_t pos, size_t length) {
    if (pos + 2 >= length) return false;
    
    return isxdigit(data[pos + 1]) && isxdigit(data[pos + 2]);
}

bool url_encoded_form_valid(char* data) {
    if (!data) return false;

    size_t length = strlen(data);
    
    if (length == 0) return true;
    
    bool expecting_key = true;
    bool found_equals = false;
    size_t key_length = 0;
    
    for (size_t i = 0; i < length; i++) {
        char c = data[i];
        
        if (!is_valid_form_char(c)) {
            return false;
        }
        
        if (c == '%') {
            if (!is_valid_percent_encoding(data, i, length)) {
                return false;
            }
            i += 2;
            continue;
        }
        
        if (c == '=') {
            if (!expecting_key || found_equals) {
                return false;
            }
            if (key_length == 0) {
                return false;
            }
            expecting_key = false;
            found_equals = true;
        }
        else if (c == '&') {
            if (expecting_key && i > 0) {
                return false;
            }
            expecting_key = true;
            found_equals = false;
            key_length = 0;
        }
        else {
            if (expecting_key) {
                key_length++;
            }
        }
    }
    
    if (expecting_key && length > 0) {
        return false;
    }
    
    return true;
}

/**
@brief checks if request body is of requested type and appropriate length
@return null

@param request_body data from request
@param body_type Content-Type header value
@param body_length Content-Length header value
*/
void body_checker(char* request_body, char* body_type, char* body_length)
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

    // If is valid or is invalid, do something
}

void parse_body(char* request_body, HTTPParserResult* struct_parser)
{
    enum HTTPMethods method = struct_parser->method;
    char* content_type = get_header_value(CONTENT_TYPE_HEADER_NAME);
    char* content_length = get_header_value(CONTENT_LENGTH_HEADER_NAME);

    if (method == POST || method == PUT || method == PATCH)
    {
        body_checker(request_body, content_type, content_length);
    }
}