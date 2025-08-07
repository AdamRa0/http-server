#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stddef.h>

typedef struct {
    char* file_content;
    size_t file_size;
    const char* operation_msg;
} FileData;

FileData read_file(const char* filepath);

#endif