#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "par.h"

char *readFile(const char *fileName)
{
	FILE *f = fopen(fileName, "rb");
	fseek(f, 0, SEEK_END);
	long l = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *dat = calloc(l + 1, 1);
	fread(dat, l, 1, f);
	dat[l] = 0;

	fclose(f);

	return dat;
}

int main(int argc, char **argv)
{
	if(argc != 2)
		return -1;

	char *src = readFile(argv[1]);
	printf("Source code:\n%s\n", src);

	printf("\nTokens:\n");
	tok_t *tokens;
	int size = lex_tokenize(src, &tokens);
	lex_dump(src, tokens, size);

	printf("\nAST:\n");
	value_t *v = par_gen_ast(src, tokens, size);
	par_ast_dump(v, 0);

	return 0;
}
