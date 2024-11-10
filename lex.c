#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

#define BLOCKSIZE 256

int lex_tokenize(const char *src, tok_t **tokens)
{
	int size = 0;
	tokens[0] = NULL;

	tok_t token = {0};

	int line = 1;
	for(int i = 0; src[i];) 
	{
		if(src[i] != '\n' && isspace(src[i]))
		{
			i++;
			continue;
		}

		else if(src[i] == '\n')
		{
			line++;
			i++;
			continue;
		}

		token.line = line;

		if(isalpha(src[i]))
		{
			token.type = TOK_KEYWORD;
			token.start = i;
			while(isalnum(src[i]))
				i++;
			token.len = i - token.start;
		}

		else if(src[i] == '-' || isdigit(src[i]))
		{
			token.type = TOK_NUMBER;
			token.start = i;
			while(isalnum(src[i]) || src[i] == '.' || src[i] == '-' || src[i] == '+')
				i++;
			token.len = i - token.start;
		}

		else if(src[i] == '"')
		{
			token.type = TOK_STRING;
			token.start = i++;

			while(src[i] != '"')
			{
				if(src[i] == '\\')
					i++;
				i++;
			}

			i++;
			token.len = i - token.start;
		}

		else if(strchr("({[", src[i]))
		{
			token.type = TOK_LBRACK;
			token.start = i++;
			token.len = 1;
		}

		else if(strchr(")}]", src[i]))
		{
			token.type = TOK_RBRACK;
			token.start = i++;
			token.len = 1;
		}

		else if(strchr(":,", src[i]))
		{
			token.type = TOK_SEP;
			token.start = i++;
			token.len = 1;
		}

		else
		{
			printf("line: %d  Unrecognized Token\n", line);
			exit(1);
		}

		if(size % BLOCKSIZE == 0)
			tokens[0] = realloc(tokens[0], sizeof(tok_t) * (BLOCKSIZE + size));
		tokens[0][size++] = token;
	}

	if(size % BLOCKSIZE == 0)
		tokens[0] = realloc(tokens[0], sizeof(tok_t) * (BLOCKSIZE + size));
	tokens[0][size++] = (tok_t){.type = TOK_END, .start = -1, .len = -1};

	return size;
}

void lex_dump(const char *str, tok_t *k, int len)
{
	static const char *keys[] = {
		"END", "KEYWORD", "STRING",
		"NUMBER", "LBRACK", "RBRACK",
		"SEPARATOR"
	};

	for(int i = 0; i < len; i++)
	{
		if(k[i].type == TOK_END)
			printf("where: %d\tline: %3d\ttype: %s\n",
					i, k[i].line, keys[k[i].type]);
		else
			printf("where: %d\tline: %3d\ttype: %s\tvalue: %.*s\n",
					i, k[i].line, keys[k[i].type], k[i].len, str + k[i].start);
	}
}
