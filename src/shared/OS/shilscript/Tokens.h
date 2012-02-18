// Tokens.h - SHilScript tokens

#ifndef _SHILSCRIPT_TOKENS_H_
#define _SHILSCRIPT_TOKENS_H_

_BEGIN_SHILSCRIPT
	
enum
{
	TK_ENDOFSCRIPT=256,	// EOF

	TK_SHL,				// <<
	TK_SHR,				// >>
	TK_INC,				// ++
	TK_DEC,				// --
	TK_LAND,			// &&
	TK_LOR,				// ||

	TK_LE,				// <=
	TK_GE,				// >=
	TK_EQUAL,			// ==
	TK_NE,				// !=
	TK_PLSE,			// +=
	TK_MNSE,			// -=
	TK_MULE,			// *=
	TK_DIVE,			// /=
	TK_MODE,			// %=
	TK_CATE,			// .=
	TK_ANDE,			// &=
	TK_ORE,				// |=
	TK_XORE,			// ^=
	TK_SHLE,			// <<=
	TK_SHRE,			// >>=

	TK_IF,				// if
	TK_ELSE,			// else
	TK_FOR,				// for
	TK_WHILE,			// while
	TK_DO,				// do
	TK_CONTINUE,		// continue
	TK_BREAK,			// break
	TK_FUNC,			// function
	TK_RETURN,			// return
	TK_VAR,				// var
	TK_PROTOTYPE,		// prototype
	TK_EXIT,			// exit

	TK_ID,				// text identifier
	TK_CONST 			// any constant value (string, number, etc.)
};



// Token type names
struct TokenName
{
	int			id;
	SSString	name;

	TokenName(int _id, SSString _name) { id=_id; name=_name; }
};

const TokenName TOKEN_NAMES[] = 
{
	TokenName(TK_ENDOFSCRIPT, "end-of-script"),

	TokenName(TK_SHL,	"<<"),
	TokenName(TK_SHR,	">>"),
	TokenName(TK_INC,	"++"),
	TokenName(TK_DEC,	"--"),
	TokenName(TK_LAND,	"&&"),
	TokenName(TK_LOR,	"||"),
	
	TokenName(TK_LE,	"<="),
	TokenName(TK_GE,	">="),
	TokenName(TK_EQUAL, "=="),
	TokenName(TK_NE,	"!="),
	TokenName(TK_PLSE,	"+="),
	TokenName(TK_MNSE,	"-="),
	TokenName(TK_MULE,	"*="),
	TokenName(TK_DIVE,	"/="),
	TokenName(TK_MODE,	"%="),
	TokenName(TK_CATE,	".="),
	TokenName(TK_ANDE,	"&="),
	TokenName(TK_ORE,	"|="),
	TokenName(TK_XORE,	"^="),
	TokenName(TK_SHLE,	"<<="),
	TokenName(TK_SHRE,	">>="),

	TokenName(TK_IF,		"if"),
	TokenName(TK_ELSE,		"else"),
	TokenName(TK_FOR,		"for"),
	TokenName(TK_WHILE,		"while"),
	TokenName(TK_DO,		"do"),
	TokenName(TK_CONTINUE,	"continue"),
	TokenName(TK_BREAK,		"break"),
	TokenName(TK_FUNC,		"function"),
	TokenName(TK_RETURN,	"return"),
	TokenName(TK_VAR,		"var"),
	TokenName(TK_PROTOTYPE,	"prototype"),
	TokenName(TK_EXIT,		"exit"),

	TokenName(TK_ID,	"identifier"),
	TokenName(TK_CONST,	"constant"),
};

const int TOKEN_NAME_COUNT = sizeof(TOKEN_NAMES)/sizeof(TokenName);


// A token, composed of the type and the associated value (numerical or string)
struct Token
{
	int			type;		// token type
	int			id;			// Constant ID associated with it
	int			line;		// line it was found on

	Token()						{ type = UNKNOWN;	id=0;	line=0;}
	Token(int Type)				{ type=Type;		id=0;	line=0;}
	Token(int Type, int ID)		{ type=Type;		id=ID;	line=0;}
};


// Gets a token's name
SSString GetTokenName(int tk);

_END_SHILSCRIPT

#endif


