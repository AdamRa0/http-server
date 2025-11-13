#ifndef QUEUE_H
#define QUEUE_H

#include "linked_list.h"

typedef struct Queue
{
    LinkedList queue;
} Queue;

typedef struct Ctx_Queue
{
    Queue* queue;
    bool initialized;
} Ctx_Queue;

Queue* init_queue(Ctx_Queue context);

BucketNode* peek(Queue* queue);

BucketNode* deque(Queue* queue);

void append(BucketNode* data, Ctx_Queue context);

void destroy_queue(Queue* queue);

#endif