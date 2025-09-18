#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stddef.h>

typedef struct {
    char* file_content;
    size_t file_size;
    const char* operation_msg;
    const char* mime_type;
} FileData;

FileData read_file(const char* filepath);

const char* add_charset(const char* mime_type);

bool starts_with_text(const char* mime_type);

#endif