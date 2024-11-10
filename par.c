#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "par.h"

double readDouble(const char *num, int len, int *pos)
{
	int i = 0;
	int mul = 1;
	double out = 0;

	if(num[0] == '-')
	{
		mul = -1;
		i++;
	}

	while(isdigit(num[i]) && i < len)
	{
		out = out * 10.0 + (num[i] - '0');
		i++;
	}

	if(num[i] == '.')
	{
		i++;
		double base = 0.1;
		while(isdigit(num[i]) && i < len)
		{
			out = out + (num[i] - '0') * base;
			base *= 0.1;
			i++;
		}
	}

	out *= mul;
	*pos = i;

	return out;
}

int readInteger(const char *num, int len)
{
	int i = 0;
	int mul = 1;
	int out = 0;
	
	if(num[0] == '-')
	{
		mul = -1;
		i++;
	}
	else if(num[0] == '+') i++;

	out = 0;

	while(isdigit(num[i]) && i < len)
	{
		out = out * 10 + (num[i] - '0');
		i++;
	}

	out *= mul;

	return out;
}

static value_t *parse_value(const char *src, tok_t *t, int *pos)
{
	if(t[*pos].type == TOK_END)
		return NULL;

	value_t *value = calloc(sizeof(value_t), 1);

	switch(t[*pos].type)
	{
		case TOK_KEYWORD:
			{
				static const char *keywords[] = { "true", "false", "null" };
				static enum value_type_t values[] = { VAL_TRUE, VAL_FALSE, VAL_NULL };

				int f = 0;
				for(unsigned long i = 0; i < sizeof(keywords)/sizeof(const char *) ; i++)
				{
					int lxn = strlen(keywords[i]);
					if(lxn == t->len && strncmp(keywords[i], src + t->start, lxn) == 0)
					{
						f = 1;
						value->type = values[i];
						break;
					}
				}

				if(!f)
				{
					printf("line: %d  Keyword not recognized.\n", t[*pos].line);
					exit(1);
				}

				++*pos;
			}
			break;

		case TOK_STRING:
			{
				value->type = VAL_STRING;
				if(t[*pos].len == 2)
					value->string.value = NULL;
				else
					value->string.value = calloc(sizeof(char), t[*pos].len);

				value->string.len = 0;

				for(int i = 1; i < t[*pos].len-1; i++)
				{
					if(src[t[*pos].start + i] == '\\')
					{
						i++;
						switch(src[t[*pos].start + i])
						{
							case '"': value->string.value[value->string.len++] = '"'; break;
							case '\\': value->string.value[value->string.len++] = '\\'; break;
							case '/': value->string.value[value->string.len++] = '/'; break;
							case 'b': value->string.value[value->string.len++] = '\b'; break;
							case 'f': value->string.value[value->string.len++] = '\f'; break;
							case 'n': value->string.value[value->string.len++] = '\n'; break;
							case 'r': value->string.value[value->string.len++] = '\r'; break;
							case 't': value->string.value[value->string.len++] = '\t'; break;
							default:
								printf("line: %d Malformed String.", t[*pos].line);
								exit(1);
						}
					}
					else
					{
						value->string.value[value->string.len++] =
							src[t[*pos].start + i];
					}
				}
				++*pos;
			}
			break;

		case TOK_NUMBER:
			{
				value->type = VAL_NUMBER;
				value->number = 0;
				int tk = 0;

				double n = readDouble(src + t[*pos].start, t[*pos].len, &tk);
				int e = 0;
				if(src[tk + t[*pos].start] == 'e' || src[tk + t[*pos].start] == 'E')
				{
					tk++;
					e = readInteger(src + tk + t[*pos].start, t[*pos].len - tk);
				}

				value->number = n * pow(10, e);
				++*pos;
			}
			break;

		case TOK_LBRACK:
			switch(src[t[*pos].start])
			{
				case '{':
					{
						value->type = VAL_OBJECT;
						++*pos;
						if(src[t[*pos].start] == '}')
						{
							++*pos;
							break;
						}

						hashmap_init(&(value->object));

						while(1)
						{
							value_t *key = parse_value(src, t, pos);

							if(src[t[*pos].start] == ':')
								++*pos;
							else
							{
								printf("line: %d Unexpected Token.\n", t[*pos].line);
								exit(1);
							}

							value_t *val = parse_value(src, t, pos);

							hashmap_insert(&(value->object), key, val);

							if(src[t[*pos].start] == ',')
							{
								++*pos;
							}
							else if(src[t[*pos].start] == '}')
							{
								++*pos;
								break;
							}
							else
							{
								printf("line: %d Unexpected Token.\n", t[*pos].line);
								exit(1);
							}
						}
					}
					break;

				case '[':
					{
						value->type = VAL_ARRAY;
						value->array = NULL;
						array_t *tail = NULL;

						++*pos;
						if(src[t[*pos].start] == ']')
						{
							++*pos;
							break;
						}

						while(1)
						{
							array_t *a = calloc(sizeof(array_t), 1);
							a->value = parse_value(src, t, pos);

							if(tail == NULL)
							{
								value->array = a;
								tail = a;
							}
							else
							{
								tail->next = a;
								tail = a;
							}

							if(src[t[*pos].start] == ',')
							{
								++*pos;
							}
							else if(src[t[*pos].start] == ']')
							{
								++*pos;
								break;
							}
							else
							{
								printf("line: %d Unexpected Token.\n", t[*pos].line);
								exit(1);
							}
						}

					}
					break;

				default:
					printf("line: %d Parsing Problem.", t[*pos].line);
					exit(1);
			}
			break;

		default:
			printf("line: %d Parsing Problem.", t[*pos].line);
			exit(1);
	}

	return value;
}

value_t *par_gen_ast(const char *src, tok_t *t, int len)
{
	int pos = 0;
	(void)len;
	value_t *out = parse_value(src, t, &pos);
	return out;
}

void par_ast_dump(value_t *v, int depth)
{
	printf("%*c", 4 * depth, '\0');
	switch(v->type)
	{
		case VAL_OBJECT:
				 {
					 printf("OBJECT:\n");
					 object_t *h = &v->object;

					 int indx = 1;

					 for(int i = 0; i < h->size; i++)
					 {
						 bucket_t *tmp = h->buckets[i];
						 while(tmp)
						 {
							 printf("%*c", 4 * depth, '\0');
							 printf("KEY %d:\n", indx++);
							 par_ast_dump(tmp->key, depth + 1);

							 printf("%*c", 4 * depth, '\0');
							 printf("VALUE:\n");
							 par_ast_dump(tmp->value, depth + 1);

							 tmp = tmp->next;
						 }
					 }
				 }
				 break;

		case VAL_ARRAY:
				 {
					 printf("ARRAY:\n");
					 array_t *a = v->array;
					 while(a)
					 {
						 par_ast_dump(a->value, depth + 1);
						 a = a->next;
					 }
				 }
				 break;

		case VAL_STRING: printf("STRING: %.*s\n", v->string.len, v->string.value); break;
		case VAL_NUMBER: printf("NUMBER: %lf\n", v->number); break;
		case VAL_TRUE: printf("TRUE\n"); break;
		case VAL_FALSE: printf("FALSE\n"); break;
		case VAL_NULL: printf("NULL\n"); break;
	}
}
