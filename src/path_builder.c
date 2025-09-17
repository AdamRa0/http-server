#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* HTML_ROOT = "./html";
const char* ERROR_PAGES = "error_pages";

char* build_path(const char* filename, bool is_error_page) {
    char* path;

    if (!is_error_page)
    {
        size_t len = strlen(HTML_ROOT) + strlen(filename) + 2;
        path = malloc(len);
        snprintf(path, len, "%s/%s", HTML_ROOT, filename);
    } else
    {
        size_t len = strlen(HTML_ROOT) + strlen(ERROR_PAGES) + strlen(filename) + 3;
        path = malloc(len);
        snprintf(path, len, "%s/%s/%s", HTML_ROOT, ERROR_PAGES, filename);
    }
    
    return path;
}