#ifndef RESPONSE_H
#define RESPONSE_H

#include "../DataStructures/hash_table.h"
#include "../Parsers/http_req_parser.h"

void set_server_response(HTTPParserResult* result, char* filename, HashTable* h_dict);

#endif