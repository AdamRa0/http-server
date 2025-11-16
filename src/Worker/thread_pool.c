#include "../DataStructures/linked_list.h"
#include "../DataStructures/queue.h"
#include "../Parsers/http_req_parser.h"
#include "thread_pool.h"

#include <pthread.h>
#include <stdlib.h>
 
void* worker(void* arg);

ThreadPool* init_thread_pool(int num_threads)
{

    ThreadPool* thread_pool = (ThreadPool* ) malloc(sizeof(ThreadPool));

    thread_pool->num_threads = num_threads;
    thread_pool->pool = (pthread_t* ) malloc(num_threads * sizeof(pthread_t));
    thread_pool->active = true;
    pthread_mutex_init(&thread_pool->lock, NULL);
    pthread_cond_init(&thread_pool->signal, NULL);

    thread_pool->work_queue = init_queue();

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_pool->pool[i], NULL, worker, thread_pool);
    }

    return thread_pool;
}

void* worker(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    
    while(pool->active)
    {
        
        pthread_mutex_lock(&pool->lock);
        void* peek_result = peek(pool->work_queue);
        
        while (peek(pool->work_queue) == NULL && pool->active)
        {
            pthread_cond_wait(&pool->signal, &pool->lock);
        }
        
        if (!pool->active)
        {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        BucketNode* job_node = (BucketNode*)deque(pool->work_queue);

        pthread_mutex_unlock(&pool->lock);
        
        if(job_node)
        {            
            ThreadJob* job = (ThreadJob* ) job_node->value;
            
            job->worker(job->buffer, job->result);
            
            free(job->buffer);
            free(job);
        }
    }
    return NULL;
}

void add_job_to_work_queue(ThreadPool* pool, ThreadJob* job)
{
    pthread_mutex_lock(&pool->lock);

    BucketNode* node = (BucketNode* ) malloc (sizeof(BucketNode));
    node->key = NULL;
    node->value = job;
    append(node, pool->work_queue);
    
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