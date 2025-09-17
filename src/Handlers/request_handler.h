#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "../DataStructures/hash_table.h"
#include "../Parsers/http_req_parser.h"

void handle_request(HTTPParserResult* result, HashTable* h_dict);

#endif