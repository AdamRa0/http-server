#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../DataStructures/queue.h"

#include <pthread.h>

typedef struct ThreadPool
{
    int num_threads;
    bool active;
    Queue* work_queue;
    pthread_t* pool;
    pthread_mutex_t lock;
    pthread_cond_t signal;
} ThreadPool;

typedef struct ThreadJob
{
    void * (* worker)(void* arg);
    void * arg;
} ThreadJob;

ThreadPool* init_thread_pool(int num_threads);

void add_job_to_work_queue(ThreadPool* pool, ThreadJob* job);

void destroy_thread_pool(ThreadPool* thread_pool);

#endif