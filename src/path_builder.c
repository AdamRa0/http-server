#include "constants.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* CONF_ROOT = "/etc/cerver/conf";

char* build_path(const char* filename, const char* file_root, const char* error_page_root, bool is_conf_file) {
    char* path;

    if (file_root)
    {
        size_t len = strlen(file_root) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", file_root, filename);
    } else if (error_page_root)
    {
        size_t len = strlen(error_page_root) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", error_page_root, filename);
    } else if (is_conf_file)
    {
        size_t len = strlen(CONF_ROOT) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", CONF_ROOT, filename);        
    } else {
        return NULL;
    }
    
    return path;
}

int is_path_safe(const char* requested_path, const char* webroot)
{
    char resolved_path[PATH_MAX];
    char resolved_webroot[PATH_MAX];

    if (realpath(requested_path, resolved_path) == NULL)
    {
        return 0;
    }

    if (realpath(webroot, resolved_webroot) == NULL)
    {
        return 0;
    }

    size_t webroot_len = strlen(resolved_webroot);

    if (strncmp(resolved_path, resolved_webroot, webroot_len) != 0)
    {
        return 0;
    }

    return 1;
}

int hex_to_int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }

    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }

    return 0;
}

char* decode_url(const char* url)
{
    size_t len = strlen(url);
    char* decoded = (char* ) malloc(len + 1);
    
    if (!decoded) return NULL;
    
    const char* src = url;
    char* dst = decoded;
    
    while (*src) {
        if (*src == '%' && isxdigit(src[1]) && isxdigit(src[2])) {
            *dst = (hex_to_int(src[1]) << 4) | hex_to_int(src[2]);
            src += 3;
            dst++;
        }
        else if (*src == '+') {
            *dst = ' ';
            src++;
            dst++;
        }
        else {
            *dst = *src;
            src++;
            dst++;
        }
    }
    *dst = '\0';
    
    return decoded;
}

int contains_traversal(const char* url) {
    if (!url) return 1;
    
    if (strstr(url, "..") != NULL) return 1;
    
    if (strncmp(url, "/etc/", 5) == 0) return 1;
    if (strncmp(url, "/usr/", 5) == 0) return 1;
    if (strncmp(url, "/var/", 5) == 0) return 1;
    if (strncmp(url, "/root/", 6) == 0) return 1;
    if (strncmp(url, "/home/", 6) == 0) return 1;
    if (strncmp(url, "/tmp/", 5) == 0) return 1;
    if (strncmp(url, "/proc/", 6) == 0) return 1;
    if (strncmp(url, "/sys/", 5) == 0) return 1;
    
    if (memchr(url, '\0', strlen(url)) != NULL) return 1;
    
    if (strchr(url, '\\') != NULL) return 1;
    
    return 0;
}

char* is_url_safe(const char* url)
{
    char* decoded_url = decode_url(url);

    if (!decoded_url)
    {
        return NULL;
    }

    if (contains_traversal(decoded_url))
    {
        free(decoded_url);
        return NULL;
    }

    return decoded_url;
}