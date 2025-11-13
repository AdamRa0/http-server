#include "linked_list.h"
#include "queue.h"

#include <stdlib.h>

Queue* init_queue(Ctx_Queue context)
{
    if (context.initialized)
    {
        return context.queue;
    }

    context.queue = (Queue* ) malloc(sizeof(Queue));
    context.initialized = true;

    return context.queue;
}

void destroy_queue(Queue* queue)
{   
    if (queue)
    {
        free(queue);
    }
}

BucketNode* peek(Queue* queue)
{
    return queue->queue.head;
}

BucketNode* deque(Queue* queue)
{
    BucketNode* prev_head = queue->queue.head;

    if (queue->queue.head->p_next != NULL)
    {
        queue->queue.head = queue->queue.head->p_next;
    } else 
    {
        return NULL;
    }

    prev_head->p_next = NULL;
    
    return prev_head;
}

void append(BucketNode* data, Ctx_Queue context)
{
    insert(data, &context.queue->queue);
}