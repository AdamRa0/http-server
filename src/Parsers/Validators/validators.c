#include "../../constants.h"
#include "http_req_parser.h"
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

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
    
    size_t boundary_len = strlen(context->boundary);

    // Validate boundary length
    if (boundary_len < MIN_BOUNDARY_LEN || boundary_len > MAX_BOUNDARY_LEN)
    {
        return;
    }

    if (isspace(conttext->boundary[boundary_len - 1])) 
    {
        return;
    }

    // is boundary RFC compliant
    for (size_t i = 0; i < boundary_len; i++) {
        unsigned char c = (unsigned char)context->boundary[i];
        if (c < 32 || c == 127) { 
            return;
        }
    }

    context->is_valid_boundary = true;

    return context->is_valid_boundary;
}

void boundary_type(Ctx_FormDataValidator* context, char* data)
{
    char* boundary_pos = strstr(data, context->boundary);
    size_t data_len = strlen(data);

    // Extract first two characters before boundary
    char first_boundary_char = data[0];
    char second_boundary_char = data[1];

    // Extract last two characters of line
    char ultimate_char = data[data_len - 1];
    char penultimate_char = data[data_len - 2];

    if (first_boundary_char == '-' && second_boundary_char == '-')
    {
        if(ultimate_char == '-' && penultimate_char == '-')
        {
            context->is_normal_boundary = false;
            context->is_end_boundary = true;
            context->is_valid_data = true;
        } else
        {
            context->is_normal_boundary = true;
        }
    }

    return;
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
    MultipartDataValidationState state = STATE_INITIAL;

    // Helper functions will check if boundary is null
    char* boundary = get_header_value(BOUNDARY_HEADER_PARAM);

    Ctx_FormDataValidator context = {
        .boundary = boundary,
        .is_content_disposition = false,
        .is_end_boundary = false,
        .is_normal_boundary = false,
        .is_valid_boundary = false,
        .is_valid_data = false,   
    };

    char* crlf = "\r\n";
    char* lf = "\n"
    char* p_line;

    char* data_dup = strdup(data);

    char* line = strtok_r(data, crlf, &p_line);

    if (line == NULL)
    {
        line = strtok_r(data, lf, &p_line);

        if (line == NULL)
        {
            free(data_dup);
            data_dup == NULL;
            return false;
        }
    }

    while (line != NULL)
    {
        switch(state)
        {
            case STATE_INITIAL:
                // We are validating data for the first time
                // Pass it on to state in boundary 
                // Boundary is first thing we expect in out data
                state = STATE_IN_BOUNDARY;
                break;
            case STATE_IN_BOUNDARY:
                is_valid_boundary(&context);

                if (!context.is_valid_boundary)
                {
                    // If line not boundary, check if line is header
                    state = STATE_IN_PART_HEADER;
                } else
                {
                    boundary_type(&context, line);

                    if (!context.is_normal_boundary)
                    {
                        // If line is end boundary, end process
                        if(context.is_end_boundary) state = STATE_FINISHED;
                    } else 
                    {
                        state = STATE_IN_PART_HEADER;
                    }
                }
                break;
            case STATE_IN_PART_HEADER:
                check_content_disposition_header_present(&context, line);

                if (context.is_content_disposition)
                {
                    state = STATE_IN_BODY;
                } else
                {
                    state = STATE_FINISHED;
                }
                break;
            case STATE_FINISHED:
                break;
            default:
                break;
        }

        if (!context.is_end_boundary)
        {
            char* line = strtok_r(data, crlf, &p_line);
    
            if (line == NULL)
            {
                line = strtok_r(data, lf, &p_line);
            }
        }
    }

    free(data_dup);
    data_dup = NULL;

    return context->is_valid_data;
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