#ifndef HEADERS_H
#define HEADERS_H

#include "../DataStructures/hash_table.h"
#include "http_req_parser.h"

void parse_header(char* header_name, char* header_value, HashTable* h_dict);

void parse_headers(char* headers, HashTable* h_dict);

char* get_header_value(const char* key, HashTable* h_dict);

void set_connection_status(HTTPParserResult* parser_struct, HashTable* h_dict);

#endif