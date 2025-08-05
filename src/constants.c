#include "constants.h"

const char* BOUNDARY_HEADER_PARAM = "boundary";
const char* CONNECTION_HEADER_NAME = "Connection";
const char* CONNECTION_KEEP_ALIVE_VALUE = "keep-alive";
const char* CONTENT_DISPOSITION_HEADER_NAME = "Content-Disposition";
const char* CONTENT_LENGTH_HEADER_NAME = "Content-Length";
const char* CONTENT_TYPE_HEADER_NAME = "Content-Type";
const char* ENTRY_TYPE_SINGLE = "single-value";
const char* ENTRY_TYPE_MULTI_PARAM = "multi-param";
const char* SERVER_ERROR_RESPONSE = "HTTP/1.1 %d %s\r\n"
    "Server: Cerver/1.0 (Linux)\r\n"
    "Date: %s\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "X-Content-Type-Options: nosniff\r\n"
    "\r\n"
    "%s";

const char* UNSUPPORTED_HTTP_VERSION_STATUS = "Unsupported HTTP Version";
const char* BAD_REQUEST_STATUS = "Bad Request";
const char* OK_STATUS = "Ok";
const char* METHOD_NOT_IMPLEMENTED_STATUS = "Method not implemented";