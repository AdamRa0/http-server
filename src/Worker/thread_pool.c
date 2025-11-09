#include "../DataStructures/queue.h"
#include "thread_pool.h"

#include <pthread.h>
#include <stdlib.h>
 
void* worker(ThreadPool* pool);

ThreadPool* init_thread_pool(int num_threads, ThreadJob job)
{
    Ctx_Queue queue_context = { NULL, false };

    ThreadPool* thread_pool = (ThreadPool* ) malloc(sizeof(ThreadPool));

    thread_pool->pool = (pthread_t* ) malloc(num_threads * sizeof(pthread_t));
    thread_pool->active = true;
    thread_pool->lock = PTHREAD_MUTEX_INITIALIZER;
    thread_pool->signal = PTHREAD_COND_INITIALIZER;

    thread_pool->work_queue = init_queue(queue_context);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_pool->pool[i], NULL, worker, NULL);
    }

    return thread_pool;
}

void worker(ThreadPool* pool)
{
    while(pool->active)
    {
        pthread_mutex_lock(&pool->lock);
        pthread_cond_wait(&pool->signal, &pool->lock);
        //TODO: fetch job from pool's work queue
        pthread_mutex_unlock(&pool->lock);
        // TODO: Execute job from pool's work queue
    }

    return NULL;
}

void add_job_to_work_queue(ThreadPool* pool, ThreadJob* job)
{
    pthread_mutex_lock(&pool->lock);
    // TODO: Add job to work queue after refactoring queue to accept generic data
    pthread_cond_signal(&pool->signal);
    pthread_mutex_unlock(&pool->lock);
}

void destroy_thread_pool(ThreadPool* thread_pool)
{
    thread_pool->active = false;

    for(int i = 0; i < thread_pool->num_threads; i++)
    {
        pthread_cond_signal(&thread_pool->signal);
    }

    for(int i = 0; i < thread_pool->num_threads; i++)
    {
        pthread_join(&thread_pool->pool[i], NULL);
    }

    free(thread_pool->pool);
    pthread_mutex_destroy(&thread_pool->lock);
    pthread_cond_destroy(&thread_pool->signal);
    destroy_queue(thread_pool->work_queue);
}