#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UNSUPPORTED_HTTP_VERSION_STATUS_CODE 505
#define BAD_REQUEST_STATUS_CODE 400
#define NOT_FOUND_STATUS_CODE 404
#define OK_STATUS_CODE 200 // Delete later
#define METHOD_NOT_IMPLEMENTED_STATUS_CODE 501
#define INTERNAL_SERVER_ERROR_STATUS_CODE 500
#define NO_CONTENT_STATUS_CODE 204

extern const char* BOUNDARY_HEADER_PARAM;
extern const char* CONNECTION_HEADER_NAME;
extern const char* CONNECTION_KEEP_ALIVE_VALUE;
extern const char* CONTENT_DISPOSITION_HEADER_NAME;
extern const char* CONTENT_LENGTH_HEADER_NAME;
extern const char* CONTENT_TYPE_HEADER_NAME;
extern const char* ENTRY_TYPE_SINGLE;
extern const char* ENTRY_TYPE_MULTI_PARAM;
extern const char* SERVER_ERROR_RESPONSE;
extern const char* SERVER_OK_RESPONSE;
extern const char* SERVER_OPTIONS_RESPONSE;
extern const char* UNSUPPORTED_HTTP_VERSION_STATUS;
extern const char* BAD_REQUEST_STATUS;
extern const char* OK_STATUS;
extern const char* NO_CONTENT_STATUS;
extern const char* NOT_FOUND_STATUS;
extern const char* METHOD_NOT_IMPLEMENTED_STATUS;
extern const char* INTERNAL_SERVER_ERROR_STATUS;
extern const char* FILE_NOT_EXISTS_ERROR;
extern const char* FILE_PERMISSIONS_ERROR;
extern const char* COULD_NOT_READ_FILE_ERROR;
extern const char* UNAUTHORIZED_FILE_OPERATION_ERROR;
extern const char* RESPONSE_TYPE_OK;
extern const char* RESPONSE_TYPE_OK_HEAD;
extern const char* RESPONSE_TYPE_ERROR;
extern const char* RESPONSE_TYPE_OPTIONS;
extern const char* SERVER_OPTIONS;

#endif