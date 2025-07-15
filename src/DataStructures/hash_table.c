#include "stdio.h"
#include <string.h>

#include "hash_table.h"
#include "linked_list.h"

#define MAGIC_NUMBER 5381

void init_hash_table(HashTable* dictionary)
{
    memset(dictionary->buckets, 0, sizeof(dictionary->buckets));
}

unsigned int hash(char* str)
{
    unsigned int hash;
    int c;

    while ((c = *str++))
    {
        hash = ((MAGIC_NUMBER << 5) + MAGIC_NUMBER) + c;
    }

    return hash;
}

void insert_to_bucket(BucketNode* node, HashTable* dictionary)
{
    char* key = node->key;
    
    int index = hash(key) % HASH_TABLE_SIZE;

    LinkedList* bucket = &dictionary->buckets[index];

    if (bucket->head == NULL)
    {
        bucket->head = node;
    } else
    {
        insert(node, bucket);
    }
}

BucketNode* find_in_bucket(char* key, HashTable* dictionary)
{
    int index = hash(key) % HASH_TABLE_SIZE;

    LinkedList* bucket = &dictionary->buckets[index];

    BucketNode* node = find(key, bucket);

    return node;
}