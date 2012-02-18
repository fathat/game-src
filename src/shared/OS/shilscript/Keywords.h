// Keywords.h - SHilScript keywords

#ifndef _SHILSCRIPT_KEYWORDS_H_
#define _SHILSCRIPT_KEYWORDS_H_

#include "Tokens.h"

_BEGIN_SHILSCRIPT

// IDs that represent keywords
const int KEYWORD_TOKENS[] = 
{
	TK_IF, TK_ELSE, TK_FOR, TK_WHILE, TK_DO, TK_CONTINUE, TK_BREAK, TK_FUNC, TK_RETURN, TK_VAR, TK_PROTOTYPE, TK_EXIT
};

const int KEYWORD_TOKEN_COUNT = sizeof(KEYWORD_TOKENS)/sizeof(int);


// Keyword type used in the lexer
struct Keyword
{
public:
	SSString	name;	// Textual name of this keyword
	int			hash;	// Hash for the name
	int			tk;		// Token type it should become
	
	Keyword() { tk=0; hash=0; }
	virtual ~Keyword() {}
};

_END_SHILSCRIPT

#endif


