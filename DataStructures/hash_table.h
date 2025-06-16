#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

typedef struct HashTable
{
    BucketNode pot[100];
} HashTable;

void insert(BucketNode node);

BucketNode find(char* key)

#endif