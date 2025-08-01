#ifndef VALIDATORS_H
#define VALIDATORS_H

#include "http_req_parser.h"
#include <ctype.h>
#include <sys/types.h>

typedef enum {
    STATE_INITIAL, // State when reading data for first time
    STATE_IN_BOUNDARY, // State when in boundary
    STATE_IN_PART_HEADER, // State when in part header and body
    STATE_IN_BODY, // Inside content of form
    STATE_FINISHED // State when validation finished
} MultipartDataValidationState;

typedef struct {
    char* boundary;
    bool is_content_disposition;
    bool is_end_boundary;
    bool is_normal_boundary; // For beginning and middle boundaries
    bool is_valid_boundary;
    bool is_valid_data;
} Ctx_FormDataValidator;

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