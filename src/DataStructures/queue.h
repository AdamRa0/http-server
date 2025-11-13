#ifndef QUEUE_H
#define QUEUE_H

#include "linked_list.h"

typedef struct Queue
{
    LinkedList queue;
} Queue;

Queue* init_queue();

BucketNode* peek(Queue* queue);

BucketNode* deque(Queue* queue);

void append(BucketNode* data, Queue* queue);

void destroy_queue(Queue* queue);

#endif