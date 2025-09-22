#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* CONF_ROOT = "./conf";
const char* HTML_ROOT = "./html";
const char* ERROR_PAGES = "error_pages";

char* build_path(const char* filename, bool is_error_page, bool is_conf_file) {
    char* path;

    if (!is_error_page && !is_conf_file)
    {
        size_t len = strlen(HTML_ROOT) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", HTML_ROOT, filename);
    } else if (is_error_page && !is_conf_file)
    {
        size_t len = strlen(HTML_ROOT) + strlen(ERROR_PAGES) + strlen(filename) + 3;
        path = malloc(len);
        snprintf(path, len, "%s/%s/%s", HTML_ROOT, ERROR_PAGES, filename);
    } else if (is_conf_file)
    {
        size_t len = strlen(CONF_ROOT) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", CONF_ROOT, filename);        
    }
    
    return path;
}