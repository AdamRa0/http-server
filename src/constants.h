#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UNSUPPORTED_HTTP_VERSION_STATUS_CODE 505
#define BAD_REQUEST_STATUS_CODE 400
#define OK_STATUS_CODE 200 // Delete later
#define METHOD_NOT_IMPLEMENTED_STATUS_CODE 501

extern const char* BOUNDARY_HEADER_PARAM;
extern const char* CONNECTION_HEADER_NAME;
extern const char* CONNECTION_KEEP_ALIVE_VALUE;
extern const char* CONTENT_DISPOSITION_HEADER_NAME;
extern const char* CONTENT_LENGTH_HEADER_NAME;
extern const char* CONTENT_TYPE_HEADER_NAME;
extern const char* ENTRY_TYPE_SINGLE;
extern const char* ENTRY_TYPE_MULTI_PARAM;
extern const char* SERVER_ERROR_RESPONSE;
extern const char* UNSUPPORTED_HTTP_VERSION_STATUS;
extern const char* BAD_REQUEST_STATUS;
extern const char* OK_STATUS;
extern const char* METHOD_NOT_IMPLEMENTED_STATUS;

#endif