#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct BucketNode
{
    char* key;
    void* value;
    struct BucketNode* p_next;
} BucketNode;

typedef struct
{
    BucketNode* head; 
} LinkedList;

void insert(BucketNode* node, LinkedList* data_structure);

BucketNode* find(const char* key, LinkedList* data_structure);

#endif