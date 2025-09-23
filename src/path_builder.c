#include "constants.h"

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