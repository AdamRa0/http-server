#ifndef BODY_PARSER_H
#define BODY_PARSER_H

#include "http_req_parser.h"

void parse_body(char* request_body, HTTPParserResult* parser_struct);

void body_checker(char* request_body, char* header_value);

ssize_t cast_char_to_ssize(char* value);

bool json_valid(char* data);

#endif