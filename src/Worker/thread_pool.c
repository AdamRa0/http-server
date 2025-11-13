#include "../DataStructures/linked_list.h"
#include "../DataStructures/queue.h"
#include "../Parsers/http_req_parser.h"
#include "thread_pool.h"

#include <pthread.h>
#include <stdlib.h>
 
void* worker(void* arg);

Ctx_Queue queue_context;

ThreadPool* init_thread_pool(int num_threads)
{
    Ctx_Queue queue_context = { NULL, false };

    ThreadPool* thread_pool = (ThreadPool* ) malloc(sizeof(ThreadPool));

    thread_pool->pool = (pthread_t* ) malloc(num_threads * sizeof(pthread_t));
    thread_pool->active = true;
    pthread_mutex_init(&thread_pool->lock, NULL);
    pthread_cond_init(&thread_pool->signal, NULL);

    thread_pool->work_queue = init_queue(queue_context);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_pool->pool[i], NULL, worker, NULL);
    }

    return thread_pool;
}

void* worker(void* arg)
{
    ThreadPool* pool = (ThreadPool* ) arg;
    while(pool->active)
    {
        pthread_mutex_lock(&pool->lock);
        pthread_cond_wait(&pool->signal, &pool->lock);

        ThreadJob* job_present = (ThreadJob* ) peek(queue_context.queue);
        if (!job_present)
        {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }

        ThreadJob* job = (ThreadJob* ) deque(queue_context.queue);
        pthread_mutex_unlock(&pool->lock);

        job->worker(job->buffer, job->result);
    }

    return NULL;
}

void add_job_to_work_queue(ThreadPool* pool, ThreadJob* job)
{
    pthread_mutex_lock(&pool->lock);
    BucketNode* node = (BucketNode* ) malloc (sizeof(BucketNode));
    node->key = NULL;
    node->value = job;
    append(node, queue_context);
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
        pthread_join(thread_pool->pool[i], NULL);
    }

    free(thread_pool->pool);
    pthread_mutex_destroy(&thread_pool->lock);
    pthread_cond_destroy(&thread_pool->signal);
    destroy_queue(thread_pool->work_queue);
}