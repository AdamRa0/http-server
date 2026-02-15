#include "linked_list.h"
#include "queue.h"

#include "../Worker/thread_pool.h"

#include <stdlib.h>

Queue* init_queue()
{

    Queue* queue = (Queue* ) malloc(sizeof(Queue));

    queue->queue.head = NULL;

    return queue;
}

void destroy_queue(Queue* queue)
{   
    if (!queue)
    {
        return;
    }

    BucketNode* current = queue->queue.head;
    while (current)
    {
        BucketNode* next = current->p_next;
        
        if (current->value)
        {
            ThreadJob* job = (ThreadJob*)current->value;
            
            if (job->buffer) free(job->buffer);
            
            if (job->result)
            {
                if (job->result->client_ip) free(job->result->client_ip);
                if (job->result->response_headers) free(job->result->response_headers);
                if (job->result->data_mime_type) free(job->result->data_mime_type);
                if (job->result->data_content) free(job->result->data_content);
                if (job->result->URI) free(job->result->URI);
                if (job->result->headers) free(job->result->headers);
                if (job->result->request_body) free(job->result->request_body);
                free(job->result);
            }
            
            free(job);
        }
        
        if (current->key) free(current->key);
        
        free(current);
        current = next;
    }
    
    free(queue);
}

BucketNode* peek(Queue* queue)
{
    if (!queue)
    {
        return NULL;
    }
    return queue->queue.head;
}

BucketNode* deque(Queue* queue)
{
    if (queue == NULL || queue->queue.head == NULL)
    {
        return NULL;
    }

    BucketNode* prev_head = queue->queue.head;

    BucketNode* future_head = queue->queue.head->p_next;

    if (future_head != NULL)
    {
        queue->queue.head = future_head;
    } else 
    {
        queue->queue.head = NULL;
    }

    prev_head->p_next = NULL;
    
    return prev_head;
}

void append(BucketNode* data, Queue* queue)
{    
    if (queue == NULL)
    {
        return;
    }
    
    insert(data, &queue->queue);
}