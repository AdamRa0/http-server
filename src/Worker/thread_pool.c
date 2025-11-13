#include "../DataStructures/linked_list.h"
#include "../DataStructures/queue.h"
#include "../Parsers/http_req_parser.h"
#include "thread_pool.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
void* worker(void* arg);

Ctx_Queue queue_context;

ThreadPool* init_thread_pool(int num_threads)
{
    queue_context.queue = NULL;
    queue_context.initialized = false;

    ThreadPool* thread_pool = (ThreadPool* ) malloc(sizeof(ThreadPool));

    thread_pool->num_threads = num_threads;
    thread_pool->pool = (pthread_t* ) malloc(num_threads * sizeof(pthread_t));
    thread_pool->active = true;
    pthread_mutex_init(&thread_pool->lock, NULL);
    pthread_cond_init(&thread_pool->signal, NULL);

    thread_pool->work_queue = init_queue(queue_context);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_pool->pool[i], NULL, worker, thread_pool);
    }

    return thread_pool;
}

void* worker(void* arg)
{
    ThreadPool* pool = (ThreadPool* ) arg;
    printf("Worker thread started\n");
    while(pool->active)
    {
        pthread_mutex_lock(&pool->lock);
        printf("Worker: waiting for job...\n"); 
        while (peek(pool->work_queue) == NULL && pool->active)
        {
            pthread_cond_wait(&pool->signal, &pool->lock);
        }

        printf("Worker: woke up\n");
        
        if (!pool->active)
        {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        ThreadJob* job = (ThreadJob* ) deque(pool->work_queue);
        pthread_mutex_unlock(&pool->lock);


        printf("Worker: got job %p\n", (void*)job);
        
        if(job)
        {
            printf("Worker: executing job\n");
            job->worker(job->buffer, job->result);
            printf("Worker: job completed\n");
            free(job->buffer);
            free(job);
        }
    }

    printf("Worker thread exiting\n");
    return NULL;
}

void add_job_to_work_queue(ThreadPool* pool, ThreadJob* job)
{
    printf("Adding job to queue\n");
    pthread_mutex_lock(&pool->lock);
    printf("Mutex lock acquired by thread\n");
    BucketNode* node = (BucketNode* ) malloc (sizeof(BucketNode));
    node->key = NULL;
    node->value = job;
    append(node, pool->work_queue);
    printf("Job added, signaling workers\n");
    pthread_cond_signal(&pool->signal);
    pthread_mutex_unlock(&pool->lock);
    printf("Mutex lock released by thread\n");
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