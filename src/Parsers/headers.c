#include "../constants.h"
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

char* get_header_value(const char* key)
{
    HashTable* h_dict = provide_hash_table();

    BucketNode* h_node = find_in_bucket(key, h_dict);

    return h_node != NULL ? h_node->value : NULL;
}

/**
@brief parses multiparam, single value and multivalue headers
@returns nothing

@param header_name Null terminated string containing header name
@param header_value Null terminated string containing header value(s)
@param h_dict pointer to hashtable datastructure to store our headers
 */
void parse_header(char* header_name, char* header_value, HashTable* h_dict)
{
    if (header_name != NULL && header_value != NULL)
    {
        const char multi_param_delim = ';';

        char* multi_param_delim_pos = strchr(header_value, multi_param_delim);

        // Header value not multiple parameters
        if (multi_param_delim_pos == NULL)
        {
            BucketNode* node = (BucketNode* ) malloc(sizeof(BucketNode));
    
            node->key = strdup(header_name);
            node->value = strdup(header_value);
    
            insert_to_bucket(node, h_dict, ENTRY_TYPE_SINGLE);
        } else 
        {
            char* p_extra_params;

            char str_multi_param_delim[2] = {multi_param_delim, '\0'};

            char* content_header_value = strtok_r(header_value, str_multi_param_delim, &p_extra_params);

            // Insert value of content_type header
            BucketNode* node = (BucketNode* ) malloc(sizeof(BucketNode));
    
            node->key = strdup(header_name);
            node->value = strdup(content_header_value);
    
            insert_to_bucket(node, h_dict, ENTRY_TYPE_SINGLE);

            char* extra_param = strtok_r(NULL, str_multi_param_delim, &p_extra_params);

            while (extra_param != NULL)
            {
                while (*extra_param == ' ') extra_param++;

                const char param_delim = '=';
                char* param_delim_pos = strchr(extra_param, param_delim);

                if (param_delim_pos != NULL)
                {
                    *param_delim_pos = '\0';

                    char* param_name = extra_param;
                    char* param_value = param_delim_pos + 1;

                    // Insert value of extra params
                    BucketNode* node = (BucketNode* ) malloc(sizeof(BucketNode));
            
                    node->key = strdup(param_name);
                    node->value = strdup(param_value);
            
                    insert_to_bucket(node, h_dict, ENTRY_TYPE_MULTI_PARAM);
                }

                extra_param = strtok_r(NULL, str_multi_param_delim, &p_extra_params);
            }
        }        
    }
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
        char* line_delim = ":";

        char* delim_pos = strstr(line, line_delim);

        *delim_pos = '\0';
        
        char* header_name = line;
        char* header_value = delim_pos + 2;

        parse_header(header_name, header_value, h_dict);

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
    char* connection_status = get_header_value(CONNECTION_HEADER_NAME);

    if (connection_status != NULL && (strcmp(connection_status, CONNECTION_KEEP_ALIVE_VALUE) == 0))
    {
        parser_struct->connection_status = KEEP_ALIVE;
    } else
    {
        parser_struct->connection_status = CLOSE;
    }
}