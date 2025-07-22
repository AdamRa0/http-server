#ifndef HEADERS_H
#define HEADERS_H

#include "../DataStructures/hash_table.h"
#include "http_req_parser.h"

HashTable* provide_hash_table();

void parse_headers(char* headers);

char* get_header_value(char* key);

void set_connection_status(HTTPParserResult* parser_struct);

#endif