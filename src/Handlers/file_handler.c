#include "../constants.h"
#include "file_handler.h"

#include <ctype.h>
#include <errno.h>
#include <magic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

    if (errno == EIO)
    {
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        return data;
    }

    size_t file_size = (size_t)st.st_size;

    file_buffer = (char* ) malloc(file_size);

    size_t res = fread(file_buffer, 1, file_size, fd);

    if (res != file_size)
    {
        data.operation_msg = COULD_NOT_READ_FILE_ERROR;
        free(file_buffer);
        fclose(fd);
        return data;
    }

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

    data.mime_type = mime_type;
    magic_close(cookie);

    return data;
}