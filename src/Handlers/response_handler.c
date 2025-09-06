#include "file_handler.h"

#include "../constants.h"
#include "../Parsers/http_req_parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define RESPONSE_SIZE 1024
#define DATE_BUFFER_SIZE 64

void set_server_response(HTTPParserResult* result, int status_code, const char* status, const char* response_type, char* filename)
{

    FileData file_data;

    if (filename)
    {
        file_data = read_file(filename);
    }

    char* response = (char* ) malloc(RESPONSE_SIZE);
    char date_buffer[DATE_BUFFER_SIZE];

    time_t now = time(NULL);
    struct tm* gmt_time = gmtime(&now);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

    if (response_type == RESPONSE_TYPE_ERROR)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE, status_code, status, date_buffer, file_data.file_size, file_data.file_content);
    }

    if (response_type == RESPONSE_TYPE_OK)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE, status_code, status, date_buffer, file_data.file_size, file_data.file_content);
    }

    if (response_type == RESPONSE_TYPE_OK_HEAD)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE, status_code, status, date_buffer, file_data.file_size, "");
    }

    if (response_type == RESPONSE_TYPE_OPTIONS)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OPTIONS_RESPONSE, status_code, status, SERVER_OPTIONS, date_buffer);
    }

    result->response_body = response;
}