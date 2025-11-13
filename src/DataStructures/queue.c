#include "linked_list.h"
#include "queue.h"

#include <stdlib.h>

Queue* init_queue()
{

    Queue* queue = (Queue* ) malloc(sizeof(Queue));

    queue->queue.head = NULL;

    return queue;
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

    if (queue->queue.head->p_next != NULL)
    {
        queue->queue.head = queue->queue.head->p_next;
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