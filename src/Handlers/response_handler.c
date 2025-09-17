#include "file_handler.h"

#include "../constants.h"
#include "../Parsers/http_req_parser.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define RESPONSE_SIZE 1024
#define DATE_BUFFER_SIZE 64


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
void set_server_response(HTTPParserResult* result, char* filename)
{

    FileData file_data;
    int status_code;
    const char* status = NULL;
    const char* response_type;

    switch(result->method)
    {
        case GET:
            if (filename)
            {
                file_data = read_file(filename);

                switch (file_data.operation_msg)
                {
                    case FILE_NOT_EXISTS_ERROR:
                        status_code = NOT_FOUND_STATUS_CODE;
                        status = NOT_FOUND_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                        break;
                    case COULD_NOT_READ_FILE_ERROR:
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                        break;
                    case FILE_PERMISSIONS_ERROR:
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                        break;
                    case UNAUTHORIZED_FILE_OPERATION_ERROR:
                        status_code = INTERNAL_SERVER_ERROR_STATUS_CODE;
                        status = INTERNAL_SERVER_ERROR_STATUS;
                        response_type = RESPONSE_TYPE_ERROR;
                        break;
                    case default:
                        status_code = OK_STATUS_CODE;
                        status = OK_STATUS;
                        response_type = RESPONSE_TYPE_OK;
                        break;
                }
            }
            break;
        case HEAD:
            status_code = OK_STATUS_CODE;
            status = OK_STATUS;
            response_type = RESPONSE_TYPE_OK_HEAD;
            break;
        case OPTIONS:
            status_code = NO_CONTENT_STATUS_CODE;
            status = NO_CONTENT_STATUS;
            response_type = RESPONSE_TYPE_OPTIONS;
            break;
        case default:
            break;
    }

    char* response = (char* ) malloc(RESPONSE_SIZE);
    char date_buffer[DATE_BUFFER_SIZE];

    time_t now = time(NULL);
    struct tm* gmt_time = gmtime(&now);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

    if (response_type == RESPONSE_TYPE_ERROR)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_ERROR_RESPONSE, status_code, status, date_buffer, file_data.mime_type, file_data.file_size, file_data.file_content);
    }

    if (response_type == RESPONSE_TYPE_OK)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE, status_code, status, date_buffer, file_data.mime_type, file_data.file_size, file_data.file_content);
    }

    if (response_type == RESPONSE_TYPE_OK_HEAD)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OK_RESPONSE, status_code, status, date_buffer, file_data.mime_type, file_data.file_size, "");
    }

    if (response_type == RESPONSE_TYPE_OPTIONS)
    {
        snprintf(response, RESPONSE_SIZE, SERVER_OPTIONS_RESPONSE, status_code, status, SERVER_OPTIONS, date_buffer);
    }

    result->response_body = response;
}