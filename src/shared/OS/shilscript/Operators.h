// Operators.h - SHilScript operator definitions

#ifndef _SHILSCRIPT_OPERATORS_H_
#define _SHILSCRIPT_OPERATORS_H_

_BEGIN_SHILSCRIPT

// Assignment operator info, for the table below
struct AssignOp
{
	int tk;
	int opcode;
};

// Table of assignment operators. This defines how an assign op token (i.e. +=, -=, etc.)
// relates to its opcode (+= means add, -= means subtract, etc.)
const AssignOp ASSIGN_OPS[] = 
{
	{TK_PLSE,	OP_ADD},
	{TK_MNSE,	OP_SUB},
	{TK_MULE,	OP_MUL},
	{TK_DIVE,	OP_DIV},
	{TK_MODE,	OP_MOD},
	{TK_CATE,	OP_CAT},
	{TK_ANDE,	OP_AND},
	{TK_ORE,	OP_OR},
	{TK_XORE,	OP_XOR},
	{TK_SHLE,	OP_SHL},
	{TK_SHRE,	OP_SHR},
};

// # of rows in the assign op table
const int ASSIGN_ROWS = sizeof(ASSIGN_OPS)/sizeof(AssignOp);

// Binary operator info (token -> opcode)
struct BinaryOp
{
	int tk;
	int opcode;
};

// # of columns in the below table
const int BINARY_COLS = 4;

// Table of binary operator precedence
const BinaryOp BINARY_OPS[][BINARY_COLS] = 
{
	{{TK_LOR,	OP_LOR},	{0,0},				{0,0},				{0,0}},
	{{TK_LAND,	OP_LAND},	{0,0},				{0,0},				{0,0}},
	{{'|',		OP_OR},		{0,0},				{0,0},				{0,0}},
	{{'^',		OP_XOR},	{0,0},				{0,0},				{0,0}},
	{{'&',		OP_AND},	{0,0},				{0,0},				{0,0}},
	{{TK_EQUAL,	OP_EQUAL},	{TK_NE,	 OP_NE},	{0,0},				{0,0}},
	{{'<',		OP_LT},		{'>',	 OP_GT},	{TK_LE,	OP_LE},		{TK_GE,	OP_GE}},	
	{{'.',		OP_CAT},	{0,0},				{0,0},				{0,0}},
	{{TK_SHL,	OP_SHL},	{TK_SHR, OP_SHR},	{0,0},				{0,0}},
	{{'+',		OP_ADD},	{'-',	 OP_SUB},	{0,0},				{0,0}},
	{{'*',		OP_MUL},	{'/',	 OP_DIV},	{'%', OP_MOD},		{0,0}},
};

// # of binary op rows
const int BINARY_ROWS = sizeof(BINARY_OPS)/(sizeof(BinaryOp)*BINARY_COLS);

_END_SHILSCRIPT

#endif


