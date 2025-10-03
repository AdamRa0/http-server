#include "../constants.h"
#include "file_handler.h"

#include <ctype.h>
#include <errno.h>
#include <magic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
@brief checks if mime type starts with text
@return bool

@param mime_type const char*
*/
bool starts_with_text(const char* mime_type)
{
    const char* NEEDLE = "text";

    return strncmp(mime_type, NEEDLE, strlen(NEEDLE)) == 0;
}

/*
@brief adds charset to mimetype;
@return const char* (newly updated mime_type)

@param mime_type const char*
*/
const char* add_charset(const char* mime_type)
{
    const char* str_to_append = "; charset=utf-8";

    size_t str_to_append_len = strlen(str_to_append);

    size_t mime_type_len = strlen(mime_type);

    char* str = (char*) malloc(mime_type_len + str_to_append_len + 1);

    if (str == NULL) return NULL;

    strcpy(str, mime_type);
    strcat(str, str_to_append);

    return str;
}

FileData read_file(const char* filepath)
{
    FileData data = {0};
    FILE* fd = NULL;
    char* file_buffer = NULL;

    struct stat st;

    errno = 0;

    if(access(filepath, F_OK) != 0)
    {
        if (errno == ENOENT)
        {
            data.operation_msg = FILE_NOT_EXISTS_ERROR;
            return data;
        }
    }


    if(access(filepath, R_OK) != 0)
    {
        if (errno == EACCES)
        {
            data.operation_msg = FILE_PERMISSIONS_ERROR;
            return data;
        }
    }

    if (stat(filepath, &st) < 0)
    {
        if (errno == EIO)
        {
            data.operation_msg = COULD_NOT_READ_FILE_ERROR;
            return data;
        }
    }

    if(!S_ISREG(st.st_mode))
    {
        data.operation_msg = UNAUTHORIZED_FILE_OPERATION_ERROR;
        return data;
    }
    
    fd = fopen(filepath, "rb");

    if (fd == NULL)
    {
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        return data;
    }

    if (errno == EIO)
    {
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        return data;
    }

    size_t file_size = (size_t)st.st_size;

    file_buffer = (char* ) malloc(file_size + 1);

    if (file_buffer == NULL)
    {
        fclose(fd);
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        return data;
    }

    size_t res = fread(file_buffer, 1, file_size, fd);

    if (res != file_size)
    {
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        free(file_buffer);
        fclose(fd);
        return data;
    }

    file_buffer[file_size] = '\0';
    data.file_content = file_buffer;
    data.file_size = res;

    fclose(fd);

    magic_t cookie = magic_open(MAGIC_MIME_TYPE);

    if (!cookie)
    {
        fprintf(stderr, "Unable to initialize magic library.\n");
        data.mime_type = NULL;

        return data;
    }

    if (magic_load(cookie, NULL) != 0)
    {
        fprintf(stderr, "Unable to load magic database: %s\n", magic_error(cookie));
        data.mime_type = NULL;
        magic_close(cookie);

        return data;
    }

    const char* mime_type = magic_file(cookie, filepath);

    if (!mime_type)
    {
        fprintf(stderr, "Unable to find file MIME type: %s\n", magic_error(cookie));
        data.mime_type = NULL;
        magic_close(cookie);

        return data;
    }

    if (strcmp(mime_type, "application/json") == 0 || strcmp(mime_type, "application/xml") == 0 || starts_with_text(mime_type))
    {
        const char* proper_mime_type = add_charset(mime_type);
    
        data.mime_type = proper_mime_type;
    } else 
    {
        data.mime_type = strdup(mime_type);
    }
    
    magic_close(cookie);

    return data;
}