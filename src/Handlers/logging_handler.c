#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t logger_lock = PTHREAD_MUTEX_INITIALIZER;

int write_log(const char* logfile_path, const char* logfile_content)
{
    FILE* fd = NULL;

    errno = 0;

    pthread_mutex_lock(&logger_lock);

    fd = fopen(logfile_path, "a");

    if (fd == NULL)
    {
        perror("Could not open file");
        return 1;
    }

    fprintf(fd, logfile_content);

    fclose(fd);

    pthread_mutex_unlock(&logger_lock);

    return 0;
}