#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

#define HASH_TABLE_SIZE 100

typedef struct HashTable
{
    LinkedList buckets[HASH_TABLE_SIZE];
} HashTable;

void init_hash_table();

int insert_to_bucket(BucketNode* node, HashTable* dictionary);

BucketNode* find_in_bucket(char* key, int index, HashTable* dictionary);

unsigned int hash(char* str);

#endif