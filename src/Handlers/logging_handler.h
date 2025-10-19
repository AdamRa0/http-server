#ifndef LOGGING_HANDLER_H
#define LOGGING_HANDLER_H

#define ERROR_LOG_FILE_PATH "/var/log/cerver/error-logs.txt"
#define ACCESS_LOG_FILE_PATH "/var/log/cerver/access-logs.txt"

int write_log(const char* logfile_path, const char* logfile_content);

#endif