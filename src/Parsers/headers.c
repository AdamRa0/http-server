#include "../DataStructures/hash_table.h"
#include "../DataStructures/linked_list.h"
#include "headers.h"
#include "http_req_parser.h"
#include <stdlib.h>
#include <string.h>

HashTable header_dictionary;
static bool initialized = false;

HashTable* provide_hash_table()
{
    if (!initialized)
    {
        init_hash_table(&header_dictionary);
        initialized = true;
    }
    return &header_dictionary;
}

char* get_header_value(char* key)
{
    HashTable* h_dict = provide_hash_table();

    BucketNode* h_node = find_in_bucket(key, h_dict);

    return h_node->value;
}

void parse_headers(char* headers)
{
    HashTable* h_dict = provide_hash_table();

    char* headers_dup = strdup(headers);

    char* p_header_line;

    char* line = strtok_r(headers_dup, "\r\n", &p_header_line);

    if (line == NULL)
    {
        line = strtok_r(headers_dup, "\n", &p_header_line);

        if (line == NULL)
        {
            free(headers_dup);
            headers_dup = NULL;
            return;
        }
    }

    while (line != NULL)
    {
        char* line_delim = ": ";

        char* delim_pos = strstr(line, line_delim);

        *delim_pos = '\0';
        
        char* header_name = line;
        char* header_value = delim_pos + 2;

        BucketNode* node = (BucketNode* ) malloc(sizeof(BucketNode));

        node->key = strdup(header_name);
        node->value = strdup(header_value);

        insert_to_bucket(node, h_dict);
        
        line = strtok_r(NULL, "\r\n", &p_header_line);

        if (line == NULL)
        {
            line = strtok_r(NULL, "\n", &p_header_line);
        }
    }

    free(headers_dup);
    headers_dup = NULL;
}

void set_connection_status(HTTPParserResult* parser_struct)
{
    char* connection_status = get_header_value("Connection");

    if ((strcmp(connection_status, "keep-alive") == 0))
    {
        parser_struct->connection_status = KEEP_ALIVE;
    } else
    {
        parser_struct->connection_status = CLOSE;
    }
}