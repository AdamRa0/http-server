#ifndef VALIDATORS_H
#define VALIDATORS_H

#include "../http_req_parser.h"
#include <ctype.h>
#include <sys/types.h>

typedef struct Ctx_FormDataValidator Ctx_FormDataValidator;


typedef enum {
    STATE_EXPECT_BOUNDARY,      // Looking for initial or intermediate boundary
    STATE_PARSE_HEADERS,        // Processing part headers
    STATE_EXPECT_EMPTY_LINE,    // Must find blank line after headers
    STATE_PARSE_BODY,          // Reading part content until next boundary
    STATE_COMPLETE             // Found end boundary, validation complete
} MultipartDataValidationState;

struct Ctx_FormDataValidator {
    char* boundary;
    size_t boundary_len;
    bool is_content_disposition;
    bool is_end_boundary;
    bool is_normal_boundary; // For beginning and middle boundaries
    bool is_valid_boundary;
    bool found_empty_line;
    bool is_valid_data;
    int segment_count;
};

bool is_empty_line(char* data);

// sets is_valid_boundary member in context struct
void is_valid_boundary(Ctx_FormDataValidator* context);

void boundary_type(Ctx_FormDataValidator* context, char* data);

void check_content_disposition_header_present(Ctx_FormDataValidator* context, char* data);

bool multipart_form_data_valid(char* data);

ssize_t cast_char_to_ssize(char* value);

bool json_valid(char* data);

bool is_valid_form_char(char c);

bool is_valid_percent_encoding(const char* data, size_t pos, size_t length);

bool url_encoded_form_valid(char* data);

#endif