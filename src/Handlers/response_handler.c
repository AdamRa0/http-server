#include "file_handler.h"
#include "logging_handler.h"

#include "../constants.h"
#include "../path_builder.h"
#include "../DataStructures/hash_table.h"
#include "../Parsers/headers.h"
#include "../Parsers/http_req_parser.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define RESPONSE_SIZE 1024
#define DATE_BUFFER_SIZE 64
#define LOG_CONTENT_SIZE 10 * 1024


/*
@brief serves static files
@returns null

@param result: Struct containing our response body
@param status_code: Which status code to serve
@param status: Which status to serve
@param response_type: Response string to serve e.g OK, Not Found
@param filename: Static file to serve
@param file_mime_type: Mime type which acts as header
*/
void set_server_response(HTTPParserResult* result, char* filename, HashTable* h_dict)
{

    FileData file_data;
    FileData not_found_data;
    const char* not_found_file = "404.html";
    const char* forbidden_file = "403.html";
    int status_code;
    const char* status = NULL;
    const char* response_type;
    const char* request_method;

    char LOG_CONTENT[LOG_CONTENT_SIZE];

    char* connection_header_value = h_dict ? get_header_value("Connection", h_dict) : NULL;

    switch(result->method)
    {
        case GET:
            if (filename)
            {
                file_data = read_file(filename);
                
                if (file_data.operation_msg)
                {
                    if (strcmp(file_data.operation_msg, FILE_NOT_EXISTS_ERROR) == 0)
                    {
                        char* temp_path = build_path(not_found_file, NULL, result->error_page_root->valuestring, false);
                        const char* not_found_error_page = temp_path;
                        not_found_data = read_file(not_found_error_page);

                        status_code = NOT_FOUND_STATUS_CODE;
                        status = NOT_FOUND_STATUS;
                        response_type = RESPONSE_TYPE_NOT_FOUND;
                    }

                    else if (strcmp(file_data.operation_msg, COULD_NOT_READ_FILE_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }

                    else if (strcmp(file_data.operation_msg, FILE_PERMISSIONS_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }

                    else if (strcmp(file_data.operation_msg, UNAUTHORIZED_FILE_OPERATION_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }
                } else 
                {
                    if (strstr(filename, forbidden_file))
                    {
                        status_code = FORBIDDEN_STATUS_CODE;
                        status = FORBIDDEN_STATUS;
                        response_type = RESPONSE_TYPE_FORBIDDEN;
                    } else
                    {                        
                        status_code = OK_STATUS_CODE;
                        status = OK_STATUS;
                        response_type = RESPONSE_TYPE_OK;
                    }
                }                
            }
            request_method = "GET";
            break;
        case HEAD:
            if (filename)
            {
                file_data = read_file(filename);
                
                if (file_data.operation_msg)
                {
                    if (strcmp(file_data.operation_msg, FILE_NOT_EXISTS_ERROR) == 0)
                    {
                        char* temp_path = build_path(not_found_file, NULL, result->error_page_root->valuestring, false);
                        const char* not_found_error_page = temp_path;
                        not_found_data = read_file(not_found_error_page);

                        status_code = NOT_FOUND_STATUS_CODE;
                        status = NOT_FOUND_STATUS;
                        response_type = RESPONSE_TYPE_NOT_FOUND;
                    }

                    else if (strcmp(file_data.operation_msg, COULD_NOT_READ_FILE_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }

                    else if (strcmp(file_data.operation_msg, FILE_PERMISSIONS_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }

                    else if (strcmp(file_data.operation_msg, UNAUTHORIZED_FILE_OPERATION_ERROR) == 0)
                    {
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                    }
                } else 
                {
                    status_code = OK_STATUS_CODE;
                    status = OK_STATUS;
                    response_type = RESPONSE_TYPE_OK_HEAD;
                }
                
            }
            request_method = "HEAD";
            break;
        case OPTIONS:
            status_code = NO_CONTENT_STATUS_CODE;
            status = NO_CONTENT_STATUS;
            response_type = RESPONSE_TYPE_OPTIONS;
            request_method = "OPTIONS";
            break;
        default:
            break;
    }

    char* response = (char* ) malloc(RESPONSE_SIZE);
    char date_buffer[DATE_BUFFER_SIZE];
    
    time_t now = time(NULL);
    struct tm* gmt_time = gmtime(&now);
    
    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
    
    if (response_type == RESPONSE_TYPE_ERROR)
    {
        result->data_mime_type = file_data.mime_type;
        result->data_content = file_data.file_content;
        result->response_size = file_data.file_size;
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE_HEADER, status_code, status, date_buffer, file_data.mime_type, file_data.file_size);

        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ERROR_LOG, date_buffer, result->client_ip, result->error_message);

        if (write_log(ERROR_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }

    if (response_type == RESPONSE_TYPE_OK)
    {
        result->data_mime_type = file_data.mime_type;
        result->data_content = file_data.file_content;
        result->response_size = file_data.file_size;
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE_HEADER, status_code, status, date_buffer, file_data.mime_type, file_data.file_size, connection_header_value);

        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ACCESS_LOG, date_buffer, result->client_ip, request_method, filename, status_code);

        if (write_log(ACCESS_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }

    if (response_type == RESPONSE_TYPE_OK_HEAD)
    {
        result->data_mime_type = file_data.mime_type;
        result->data_content = NULL;
        result->response_size = file_data.file_size;
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE_HEADER, status_code, status, date_buffer, file_data.mime_type, file_data.file_size, connection_header_value);

        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ACCESS_LOG, date_buffer, result->client_ip, request_method, filename, status_code);

        if (write_log(ACCESS_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }

    if (response_type == RESPONSE_TYPE_OPTIONS)
    {
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_OPTIONS_RESPONSE_HEADER, status_code, status, SERVER_OPTIONS, date_buffer, connection_header_value);

        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ACCESS_LOG, date_buffer, result->client_ip, request_method, filename, status_code);

        if (write_log(ACCESS_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }

    if (response_type == RESPONSE_TYPE_NOT_FOUND)
    {
        result->data_mime_type = not_found_data.mime_type;
        result->data_content = not_found_data.file_content;
        result->response_size = not_found_data.file_size;
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE_HEADER, status_code, status, date_buffer, not_found_data.mime_type, not_found_data.file_size);
        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ERROR_LOG, date_buffer, result->client_ip, "File not found");

        if (write_log(ERROR_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }

    if (response_type == RESPONSE_TYPE_FORBIDDEN)
    {
        result->data_mime_type = file_data.mime_type;
        result->data_content = file_data.file_content;
        result->response_size = file_data.file_size;
        result->response_headers_size = snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE_HEADER, status_code, status, date_buffer, file_data.mime_type, file_data.file_size);
        snprintf(LOG_CONTENT, LOG_CONTENT_SIZE, ERROR_LOG, date_buffer, result->client_ip, result->error_message);

        if (write_log(ERROR_LOG_FILE_PATH, LOG_CONTENT) > 0) perror("Failed to write logs");
    }    

    result->response_headers = response;
}