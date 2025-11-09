#ifndef QUEUE_H
#define QUEUE_H

#include "linked_list.h"

typedef struct Queue
{
    LinkedList queue;
} Queue;

typedef struct Ctx_queue
{
    Queue* queue;
    bool initialized;
} Ctx_queue;

Queue* init_queue(Ctx_queue context;);

BucketNode* peak(Queue* queue);

BucketNode* deque(Queue* queue);

void append(BucketNode* data);

void destroy_queue(Queue* queue);

#endif