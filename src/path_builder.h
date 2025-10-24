#ifndef PATH_BUILDER_H
#define PATH_BUILDER_H

char* build_path(const char* filename, const char* file_root, const char* error_page_root, bool is_conf_file);

int is_path_safe(const char* requested_path, const char* webroot);

int hex_to_int(char c);

char* decode_url(const char* url);

int contains_traversal(const char* url);

char* is_url_safe(const char* url);

#endif