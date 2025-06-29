#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct BucketNode BucketNode;

struct BucketNode
{
    char* key;
    char* value;
    BucketNode* p_next;
};

typedef struct LinkedList
{
    BucketNode* head; 
} LinkedList;

void insert(BucketNode* node, LinkedList* data_structure);

BucketNode* find(char* key, LinkedList* data_structure);

#endif