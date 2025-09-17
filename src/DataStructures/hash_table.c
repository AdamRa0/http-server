#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "../constants.h"
#include "hash_table.h"
#include "linked_list.h"

#define MAGIC_NUMBER 5381

void init_hash_table(HashTable* dictionary)
{
    memset(dictionary->buckets, 0, sizeof(dictionary->buckets));
}

void clear_hash_table(HashTable* dictionary)
{
    for(int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        BucketNode* current = dictionary->buckets[i].head;

        if (current == NULL) continue;

        while(current)
        {
            BucketNode* next = current->p_next;

            free(current->key);
            free(current->value);
            free(current);

            current = next;
        }

        dictionary->buckets[i].head = NULL;
    }
}

unsigned int hash(const char* str)
{
    unsigned int hash;
    int c;

    while ((c = *str++))
    {
        hash = ((MAGIC_NUMBER << 5) + MAGIC_NUMBER) + c;
    }

    return hash;
}

void insert_to_bucket(BucketNode* node, HashTable* dictionary, const char* entry_type)
{
    char* key = node->key;
    
    int index = hash(key) % HASH_TABLE_SIZE;

    LinkedList* bucket = &dictionary->buckets[index];

    if (bucket->head == NULL)
    {
        bucket->head = node;
        return;
    }

    if (strcmp(entry_type, ENTRY_TYPE_SINGLE) == 0)
    {
        free(bucket->head);

        bucket->head = node;
    } else if (strcmp(entry_type, ENTRY_TYPE_MULTI_PARAM) == 0)
    {
        insert(node, bucket);
    }
}

BucketNode* find_in_bucket(const char* key, HashTable* dictionary)
{
    int index = hash(key) % HASH_TABLE_SIZE;

    LinkedList* bucket = &dictionary->buckets[index];

    BucketNode* node = find(key, bucket);

    return node;
}