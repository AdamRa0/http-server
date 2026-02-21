#include "../constants.h"
#include "file_handler.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
@brief checks and returns file mime type from extension
@return const char*

@param filepath const char*
*/
const char* get_mime_type_from_extension(const char* filepath)
{
    const char* ext = strrchr(filepath, '.');
    if (!ext) return "application/octet-stream";
    
    ext++;
    
    if (strcasecmp(ext, "html") == 0 || strcasecmp(ext, "htm") == 0)
        return "text/html";
    else if (strcasecmp(ext, "css") == 0)
        return "text/css";
    else if (strcasecmp(ext, "js") == 0)
        return "text/javascript";
    else if (strcasecmp(ext, "json") == 0)
        return "application/json";
    else if (strcasecmp(ext, "xml") == 0)
        return "application/xml";
    else if (strcasecmp(ext, "png") == 0)
        return "image/png";
    else if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0)
        return "image/jpeg";
    else if (strcasecmp(ext, "gif") == 0)
        return "image/gif";
    else if (strcasecmp(ext, "svg") == 0)
        return "image/svg+xml";
    else if (strcasecmp(ext, "webp") == 0)
        return "image/webp";
    else if (strcasecmp(ext, "ico") == 0)
        return "image/x-icon";
    else if (strcasecmp(ext, "pdf") == 0)
        return "application/pdf";
    else if (strcasecmp(ext, "zip") == 0)
        return "application/zip";
    else if (strcasecmp(ext, "tar") == 0)
        return "application/x-tar";
    else if (strcasecmp(ext, "gz") == 0)
        return "application/gzip";
    else if (strcasecmp(ext, "mp4") == 0)
        return "video/mp4";
    else if (strcasecmp(ext, "mp3") == 0)
        return "audio/mpeg";
    else if (strcasecmp(ext, "wav") == 0)
        return "audio/wav";
    else if (strcasecmp(ext, "woff") == 0)
        return "font/woff";
    else if (strcasecmp(ext, "woff2") == 0)
        return "font/woff2";
    else if (strcasecmp(ext, "ttf") == 0)
        return "font/ttf";
    else if (strcasecmp(ext, "otf") == 0)
        return "font/otf";
    else if (strcasecmp(ext, "txt") == 0)
        return "text/plain";
    else
        return "application/octet-stream";
}

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
        data.operation_msg = (errno == EIO) ? COULD_NOT_READ_FILE_ERROR : COULD_NOT_READ_FILE_ERROR;
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

    const char* mime_type = get_mime_type_from_extension(filepath);

    if (!mime_type)
    {
        fprintf(stderr, "Unable to find file MIME type\n");
        data.mime_type = NULL;

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
    
    return data;
}