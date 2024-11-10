#ifndef _LEX_H_
#define _LEX_H_

typedef struct tok_t
{
	enum tok_type_t {
		TOK_END,			// probably don't need this
		TOK_KEYWORD,			// keywords: true, false, null
		TOK_STRING,			// anything under ""
		TOK_NUMBER,			// number ig
		TOK_LBRACK,			// ( [ {
		TOK_RBRACK,			// ) ] }
		TOK_SEP,			// : ,
		TOK_NEWLINE,
	} type;

	int line;
	int start;
	int len;
} tok_t;

int lex_tokenize(const char *src, tok_t **tokens);
void lex_dump(const char *src, tok_t *k, int len);

#endif
