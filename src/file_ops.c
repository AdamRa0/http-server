#include "constants.h"
#include "file_ops.h"

#include <ctype.h>
#include <errno.h>
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

    access(filepath, F_OK);

    if (errno == ENOENT)
    {
        data.operation_msg = FILE_NOT_EXISTS_ERROR;
        return data;
    }

    access(filepath, R_OK);

    if (errno == EACCES)
    {
        data.operation_msg = FILE_PERMISSIONS_ERROR;
        return data;
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
        fclose(fd);
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

    return data;
}