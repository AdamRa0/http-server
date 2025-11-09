#include "linked_list.h"
#include "queue.h"

#include <stdlib.h>

Queue* init_queue(Ctx_queue context)
{
    if (context.initialized)
    {
        return context.queue;
    }

    context.queue = (Queue* ) malloc(sizeof(Queue));

    return context.queue;
}

void destroy_queue(Queue* queue)
{   
    if (queue)
    {
        free(queue);
    }
}

BucketNode* peak(Queue* queue)
{
    return queue->head;
}

BucketNode* deque(Queue* queue)
{
    BucketNode* prev_head = queue->head;

    if (queue->head->p_next != NULL)
    {
        queue->head = queue->head->p_next;
    }

    prev_head->p_next = NULL;
    
    return prev_head;
}

void append(BucketNode* data, Ctx_queue context)
{
    insert(data, context.queue);
}