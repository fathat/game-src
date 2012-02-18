// Opcodes.h - opcode related schtuff

#ifndef _SHILSCRIPT_OPCODES_H_
#define _SHILSCRIPT_OPCODES_H_

#include "Value.h"
#include "Vector.h"

_BEGIN_SHILSCRIPT

// Opcodes
enum OPCODES
{
	// Nothing
	OP_NOOP=0,

	// Stack
	OP_PUSH,	// pushes a constant value onto the stack
	OP_POP,		// pops a value from the top of the stack
	OP_DUP,		// duplicates what's on the top of the stack

	// Unary
	OP_NOT,
	OP_POS,
	OP_NEG,
	OP_COMP,	// bitwise complement
	OP_INC,		// increments top of stack by 1
	OP_DEC,		// decrements top of stack by 1

	// Arithmetic
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_SHL,
	OP_SHR,
	OP_CAT,		// string concatenate
	OP_AND,	
	OP_OR,
	OP_XOR,
	OP_LAND,	// logical and
	OP_LOR,		// logical or
	OP_LT,		// less than
	OP_GT,		// greater than
	OP_LE,		// less than or equal to
	OP_GE,		// greater than or equal to
	OP_EQUAL,	// equality
	OP_NE,		// not equal

	// Functions
	OP_CALL,	// Calls function (by func id)
	OP_RETURN,	// Return to the last function call point

	// Vars
	OP_SETVAR,	// set var's value (by var id)
	OP_GETVAR,	// get var's value (by var id)

	// Logic & jumps
	OP_JMP,		// jump to label
	OP_JT,		// jump to label if true
	OP_JF,		// jump to label if false

	// Other
	OP_DONE 	// signifies completion
};


// Opcode names in text, for convenience.
// Not used as of 0.1.1b, but may be used in the future.
const SSString OPCODE_NAMES[] = 
{
	"NOOP",

	"PUSH",
	"POP",
	"DUP",

	"NOT",
	"POS",
	"NEG",
	"COMP",
	"INC",
	"DEC",

	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"MOD",
	"SHL",
	"SHR",
	"CAT",
	"AND",
	"OR",
	"XOR",
	"LAND",
	"LOR",
	"LT",
	"GT",
	"LE",
	"GE",
	"EQUAL",
	"NE",

	"CALL",
	"RETURN",

	"SETVAR",
	"GETVAR",

	"JMP",
	"JT",
	"JF",

	"DONE",
};

// Opcode base class
struct Opcode
{
	int		code;	// Opcode ID
	int		line;	// Line # this opcode was generated from
	int		val;	// Associated value

	Opcode() { code = line = val = 0; }
	Opcode(int Code, int Line, int Val) { code=Code; line=Line; val=Val; }
};

_END_SHILSCRIPT

#endif


