#include "../../constants.h"
#include "../../cJSON/cJSON.h"
#include "../headers.h"
#include "../http_req_parser.h"
#include "validators.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

bool is_empty_line(char* data) {
    if (!data) return true;

    while (*data) 
    {
        if (*data != ' ' && *data != '\t' && *data != '\r') 
        {
            return false;
        }
        data++;
    }
    return true;
}

void is_valid_boundary(Ctx_FormDataValidator* context)
{
    const size_t MIN_BOUNDARY_LEN = 1;
    const size_t MAX_BOUNDARY_LEN = 70;
    
    // If boundary was not stored while parsing header
    // malformed request hence not valid
    if (context->boundary == NULL)
    {
        return;
    }

    // Validate boundary length
    if (context->boundary_len < MIN_BOUNDARY_LEN || context->boundary_len > MAX_BOUNDARY_LEN)
    {
        return;
    }

    if (isspace(context->boundary[context->boundary_len - 1])) 
    {
        return;
    }

    // is boundary RFC compliant
    for (size_t i = 0; i < context->boundary_len; i++) {
        unsigned char c = (unsigned char)context->boundary[i];
        if (c < 32 || c == 127) { 
            return;
        }
    }

    context->is_valid_boundary = true;
}

/*
@brief checks for type and validity of boundary in line
If line not boundary, set is_valid_boundary in context to false
If line is boundary, set is_valid_boundary in context to true
If line is boundary, set type of boundary: is_end_boundary or is_normal_boundary
*/
void boundary_type(Ctx_FormDataValidator* context, char* data)
{
    context->is_valid_boundary = false;
    context->is_normal_boundary = false;
    context->is_end_boundary = false;

    if (!data || !context || !context->boundary) {
        return;
    }

    size_t data_len = strlen(data);
    size_t boundary_len = context->boundary_len;

    if (data_len < 2 + boundary_len) {
        return;
    }

    if (data[0] != '-' || data[1] != '-') {
        return;
    }

    if (strncmp(data + 2, context->boundary, boundary_len) != 0) {
        return;
    }

    size_t boundary_end_pos = 2 + boundary_len;

    if (boundary_end_pos == data_len) {
        context->is_valid_boundary = true;
        context->is_normal_boundary = true;
    }
    else if (boundary_end_pos + 2 == data_len && 
             data[boundary_end_pos] == '-' && 
             data[boundary_end_pos + 1] == '-') {
        context->is_valid_boundary = true;
        context->is_end_boundary = true;
        context->is_normal_boundary = false;
    }
    else {
        context->is_valid_boundary = false;
    }
}

void check_content_disposition_header_present(Ctx_FormDataValidator* context, char* data)
{
    char* content_disposition_pos = strstr(data, CONTENT_DISPOSITION_HEADER_NAME);

    if (content_disposition_pos == NULL)
    {
        context->is_content_disposition = false;
        return;
    }
    
    context->is_content_disposition = true;
}

bool multipart_form_data_valid(char* data)
{
    MultipartDataValidationState state = STATE_EXPECT_BOUNDARY;

    bool validation_failed = false;

    // Helper functions will check if boundary is null
    char* boundary = get_header_value(BOUNDARY_HEADER_PARAM);

    size_t boundary_len = strlen(boundary);

    // Remove quotes from boundary if it has any
    if (boundary[0] == '"' && boundary[boundary_len - 1] == '"')
    {
        boundary[boundary_len - 1] = '\0';
        boundary++;
    }

    Ctx_FormDataValidator context = {
        .boundary = boundary,
        .boundary_len = strlen(boundary),
        .is_content_disposition = false,
        .is_end_boundary = false,
        .is_normal_boundary = false,
        .is_valid_boundary = false,
        .is_valid_data = false,   
        .segment_count = 0,
    };

    char* p_line;

    char* data_dup = strdup(data);

    char* line = strtok_r(data_dup, "\n", &p_line);

    if (line && strlen(line) > 0 && line[strlen(line) - 1] == '\r') 
    {
        line[strlen(line) - 1] = '\0';
    }

    while (line != NULL && !validation_failed)
    {
        switch(state)
        {
            case STATE_EXPECT_BOUNDARY:
                // Check if boundary we received in request header is valid
                is_valid_boundary(&context);

                if (!context.is_valid_boundary)
                {
                    validation_failed = true;
                    break;
                }

                boundary_type(&context, line);

                // If end boundary at beginning fail validation
                if (context.is_end_boundary) 
                {
                    validation_failed = true;
                } else if (context.is_normal_boundary) 
                {
                    state = STATE_PARSE_HEADERS;
                    context.is_content_disposition = false;
                    context.found_empty_line = false;
                } else {
                    validation_failed = true;
                }                
                break;
            case STATE_PARSE_HEADERS:
                if (is_empty_line(line)) 
                {
                    if (!context.is_content_disposition) 
                    {
                        validation_failed = true;
                        break;
                    }
                    context.found_empty_line = true;
                    state = STATE_PARSE_BODY;
                } else {
                    if (!context.is_content_disposition) 
                    {
                        check_content_disposition_header_present(&context, line);
                        if (!context.is_content_disposition) 
                        {
                            validation_failed = true;
                            break;
                        }
                    }
                }
                break;
            case STATE_PARSE_BODY:
                boundary_type(&context, line);
                
                // If line is boundary
                if (context.is_valid_boundary) 
                {
                    context.segment_count++;
                    
                    if (context.is_end_boundary) 
                    {
                        state = STATE_COMPLETE;
                        context.is_valid_data = true;
                    } else if (context.is_normal_boundary) 
                    {
                        state = STATE_PARSE_HEADERS;
                        context.is_content_disposition = false;
                        context.found_empty_line = false;
                    } else 
                    {
                        validation_failed = true;
                    }
                }
                break;
            case STATE_COMPLETE:
                if (!is_empty_line(line)) {
                    validation_failed = true;
                }
                break;
            default:
                validation_failed = true;
                break;
        }

        line = strtok_r(NULL, "\n", &p_line);

        if (line && strlen(line) > 0 && line[strlen(line) - 1] == '\r') 
        {
            line[strlen(line) - 1] = '\0';
        }
    }

    free(data_dup);
    data_dup = NULL;

    return context.is_valid_data && (state == STATE_COMPLETE) && context.segment_count > 0 && !validation_failed;
}

ssize_t cast_char_to_ssize(char* value)
{
    errno = 0;

    int base = 10;
    char* p_value;

    long val = strtol(value, &p_value, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) 
    {
        perror("strtol");
        return -1;
    }

    if (p_value == value) 
    {
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

bool is_valid_form_char(char c) 
{
    return isalnum(c) || 
           c == '%' || c == '+' || c == '=' || c == '&' || 
           c == '-' || c == '_' || c == '.' || c == '~' ||
           c == ':' || c == '/' || c == '?' || c == '#' ||
           c == '[' || c == ']' || c == '@' || c == '!' ||
           c == '$' || c == '\'' || c == '(' || c == ')' ||
           c == '*' || c == ',' || c == ';';
}

bool is_valid_percent_encoding(const char* data, size_t pos, size_t length) 
{
    if (pos + 2 >= length) return false;
    
    return isxdigit(data[pos + 1]) && isxdigit(data[pos + 2]);
}

bool url_encoded_form_valid(char* data) 
{
    if (!data) return false;

    size_t length = strlen(data);
    
    if (length == 0) return true;
    
    bool expecting_key = true;
    bool found_equals = false;
    size_t key_length = 0;
    
    for (size_t i = 0; i < length; i++) {
        char c = data[i];
        
        if (!is_valid_form_char(c)) 
        {
            return false;
        }
        
        if (c == '%') 
        {
            if (!is_valid_percent_encoding(data, i, length)) 
            {
                return false;
            }
            i += 2;
            continue;
        }
        
        if (c == '=') 
        {
            if (!expecting_key || found_equals) 
            {
                return false;
            }
            if (key_length == 0) 
            {
                return false;
            }
            expecting_key = false;
            found_equals = true;
        }
        else if (c == '&') 
        {
            if (expecting_key && i > 0) 
            {
                return false;
            }
            expecting_key = true;
            found_equals = false;
            key_length = 0;
        }
        else 
        {
            if (expecting_key) 
            {
                key_length++;
            }
        }
    }
    
    if (expecting_key && length > 0) 
    {
        return false;
    }
    
    return true;
}