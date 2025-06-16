#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct BucketNode
{
    char* key;
    char* value;
    BucketNode* next;
} BucketNode

typedef struct LinkedList
{
    BucketNode* head; 
} LinkedList;

void insert(BucketNode node);

BucketNode find(char* key);

#endif