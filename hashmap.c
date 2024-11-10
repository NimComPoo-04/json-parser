#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashmap.h"
#include "par.h"

#define LOAD_FACTOR 0.8
#define BLOCK_SIZE 1024

void hashmap_init(hashmap_t *h)
{
	h->buckets = calloc(sizeof(bucket_t *), BLOCK_SIZE);
	h->size = BLOCK_SIZE;
	h->length = 0;
}

static int get_hash(hashmap_t *h, value_t *key)
{
	int hash = 0;
	int p = 1;

	for(int i = 0; i < key->string.len; i++)
	{
		hash = (hash + key->string.value[i] * p) % h->size;
		p = (p * 31) % h->size;
	}

	return hash;
}

// TODO: if ratios become less than load factor resize the thing
void hashmap_insert(hashmap_t *h, value_t *key, value_t *value)
{
	int hash = get_hash(h, key);
	bucket_t *next = h->buckets[hash];

	h->buckets[hash] = calloc(sizeof(bucket_t), 1);
	h->buckets[hash]->key = key;
	h->buckets[hash]->value = value;
	h->buckets[hash]->next = next;

	h->length++;
}

value_t *hashmap_get(hashmap_t *h, value_t *key)
{
	int hash = get_hash(h, key);
	bucket_t *tmp = h->buckets[hash];

	while(tmp)
	{
		if(key->string.len == tmp->key->string.len &&
				strncmp(key->string.value, tmp->key->string.value, key->string.len) == 0)
		{
			return tmp->value;
		}
	}

	return NULL;
}

void hashmap_foreach(hashmap_t *h, void(*func)(bucket_t *b, value_t *value))
{
	for(int i = 0; i < h->size; i++)
	{
		bucket_t *tmp = h->buckets[i];
		while(tmp)
		{
			func(tmp, tmp->value);
			tmp = tmp->next;
		}
	}
}
