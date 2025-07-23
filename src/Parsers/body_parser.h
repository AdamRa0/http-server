#ifndef BODY_PARSER_H
#define BODY_PARSER_H

#include "http_req_parser.h"

#include <sys/types.h>


typedef bool (*parsing_functions)(char* body);

extern char* supported_content_types[];
extern parsing_functions functions[];

void parse_body(char* request_body, HTTPParserResult* parser_struct);

void body_checker(char* request_body, char* body_type, char* body_length);

ssize_t cast_char_to_ssize(char* value);

bool json_valid(char* data);

bool multipart_form_data_valid(char* data);

bool url_encoded_form_valid(char* data);


#endif