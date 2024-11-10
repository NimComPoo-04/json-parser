#ifndef _HASHMAP_H_
#define _HASHMAP_H_

typedef struct value_t value_t;

typedef struct bucket_t
{
	value_t *key;
	value_t *value;
	struct bucket_t *next;
} bucket_t;

typedef struct
{
	bucket_t **buckets;
	int size;
	int length;
} hashmap_t;

void hashmap_init(hashmap_t *h);
void hashmap_insert(hashmap_t *h, value_t *key, value_t *value);
value_t *hashmap_get(hashmap_t *h, value_t *key);

void hashmap_foreach(hashmap_t *h, void(*func)(bucket_t *b, value_t *value));

#endif
