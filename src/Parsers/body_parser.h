#ifndef BODY_PARSER_H
#define BODY_PARSER_H

#include "../DataStructures/hash_table.h"
#include "http_req_parser.h"

#include <sys/types.h>


typedef bool (*parsing_functions)(char* body, HashTable* h_dict);

extern char* supported_content_types[];
extern parsing_functions functions[];

void parse_body(char* request_body, HTTPParserResult* parser_struct, HashTable* h_dict);

void body_checker(char* request_body, char* body_type, char* body_length, HTTPParserResult* parser_struct, HashTable* h_dict);

#endif