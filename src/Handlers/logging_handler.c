#include <errno.h>
#include <stdio.h>
#include <time.h>

int write_log(const char* logfile_path, const char* logfile_content)
{
    FILE* fd = NULL;

    errno = 0;

    fd = fopen(logfile_path, "a");

    if (fd == NULL)
    {
        perror("Could not open file");
        return 1;
    }

    fprintf(fd, logfile_content);

    fclose(fd);

    return 0;
}