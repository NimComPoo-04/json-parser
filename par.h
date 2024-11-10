#ifndef _PAR_H_
#define _PAR_H_

#include "lex.h"
#include "hashmap.h"

typedef struct string_t {
	char *value;
	int len;
} string_t;

typedef hashmap_t object_t;

typedef struct array_t {
	struct value_t *value;
	struct array_t *next;
} array_t;

typedef double number_t;

typedef struct value_t
{
	enum value_type_t
	{
		VAL_STRING,
		VAL_NUMBER,
		VAL_OBJECT,
		VAL_ARRAY,
		VAL_TRUE,
		VAL_FALSE,
		VAL_NULL
	} type;

	union
	{
		string_t string;
		object_t object;
		array_t  *array;
		number_t number;
	};
} value_t;

value_t *par_gen_ast(const char *src, tok_t *t, int len);
void par_ast_dump(value_t *, int);

#endif
