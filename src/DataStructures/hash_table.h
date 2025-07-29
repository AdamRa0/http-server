#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

#define HASH_TABLE_SIZE 100

typedef struct HashTable HashTable;

struct HashTable
{
    LinkedList buckets[HASH_TABLE_SIZE];
};

void init_hash_table(HashTable* dictionary);

void insert_to_bucket(BucketNode* node, HashTable* dictionary, const char* entry_type);

BucketNode* find_in_bucket(const char* key, HashTable* dictionary);

unsigned int hash(const char* str);

#endif