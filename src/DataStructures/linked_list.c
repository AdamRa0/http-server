#include "stdio.h"
#include "linked_list.h"

#include <stdlib.h>
#include <string.h>

void insert(BucketNode* node, LinkedList* data_structure)
{
    BucketNode* current_node = data_structure->head;

    while (current_node->p_next != NULL)
    {
        current_node = current_node->p_next;
    }
    current_node->p_next = node;    
}

BucketNode* find(const char* key, LinkedList* data_structure)
{
    if (data_structure->head == NULL)
    {
        return NULL;
    }

    if (strcmp(data_structure->head->key, key) == 0)
    {
        return data_structure->head;
    }

    BucketNode* current_node = data_structure->head;

    while (current_node->p_next != NULL)
    {
        if (strcmp(current_node->key, key) == 0)
        {
            return current_node;
        }

        current_node = current_node->p_next;
    }

    return NULL;
}