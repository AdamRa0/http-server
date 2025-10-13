#include "constants.h"

const char* BOUNDARY_HEADER_PARAM = "boundary";
const char* CONNECTION_HEADER_NAME = "Connection";
const char* CONNECTION_KEEP_ALIVE_VALUE = "keep-alive";
const char* CONTENT_DISPOSITION_HEADER_NAME = "Content-Disposition";
const char* CONTENT_LENGTH_HEADER_NAME = "Content-Length";
const char* CONTENT_TYPE_HEADER_NAME = "Content-Type";
const char* ENTRY_TYPE_SINGLE = "single-value";
const char* ENTRY_TYPE_MULTI_PARAM = "multi-param";
const char* SERVER_ERROR_RESPONSE_HEADER = "HTTP/1.1 %d %s\r\n"
    "Server: Cerver/1.0 (Linux)\r\n"
    "Date: %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "X-Content-Type-Options: nosniff\r\n"
    "\r\n";

const char* SERVER_OK_RESPONSE_HEADER = "HTTP/1.1 %d %s\r\n"
    "Server: Cerver/1.0 (Linux)\r\n"
    "Date: %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "Connection: %s\r\n"
    "X-Content-Type-Options: nosniff\r\n"
    "\r\n";

const char* SERVER_OPTIONS_RESPONSE_HEADER = "HTTP/1.1 %d %s\r\n"
    "Allow: %s\r\n"
    "Date: %s\r\n"
    "Server: Cerver/1.0 (Linux)\r\n"
    "Connection: %s\r\n"
    "X-Content-Type-Options: nosniff\r\n"
    "\r\n";

const char* UNSUPPORTED_HTTP_VERSION_STATUS = "Unsupported HTTP Version";
const char* BAD_REQUEST_STATUS = "Bad Request";
const char* OK_STATUS = "Ok";
const char* NO_CONTENT_STATUS = "No Content";
const char* NOT_FOUND_STATUS = "Not Found";
const char* METHOD_NOT_IMPLEMENTED_STATUS = "Method not implemented";
const char* INTERNAL_SERVER_ERROR_STATUS = "Internal Server Error";
const char* FILE_NOT_EXISTS_ERROR = "File does not exist";
const char* FILE_PERMISSIONS_ERROR = "You do not have the necessary permissions for this file";
const char* COULD_NOT_READ_FILE_ERROR = "Could not read file";
const char* UNAUTHORIZED_FILE_OPERATION_ERROR = "Not authorized to open this file";
const char* RESPONSE_TYPE_OK = "ok";
const char* RESPONSE_TYPE_OK_HEAD = "ok_head";
const char* RESPONSE_TYPE_ERROR = "error";
const char* RESPONSE_TYPE_OPTIONS = "options";
const char* RESPONSE_TYPE_NOT_FOUND = "not_found";
const char* SERVER_OPTIONS = "GET, PUT, PATCH, POST, DELETE, OPTIONS, HEAD";