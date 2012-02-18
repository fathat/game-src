// Function.h - Function table entries

#ifndef _SHILSCRIPT_FUNCTION_H_
#define _SHILSCRIPT_FUNCTION_H_

_BEGIN_SHILSCRIPT

const unsigned int MAX_FUNCS = (1<<16);

// Function ptr to a C function for use in script
typedef int (_SHILSCRIPT_CALL * FUNC)(State*, int, void*);

// Different limit types (meaning of the param count limit)
enum PARAM_LIMIT
{
	PARAMS_MIN=0,	// Param limit is a minimum # of params
	PARAMS_MAX,		// Param limit is a maximum # of params
	PARAMS_EXACT	// Param limit is the exact # of params
};

// Different function types
enum
{
	FUNC_C=0,			// A bound C function (like an API call)
	FUNC_SHILSCRIPT		// A shilscript function, defined in SHilScript code
};

// Stores info about a function
struct Function
{
	SSString	name;			// Function name (lowercase)
	int			type;			// Function type (C/shilscript)
	int			params;			// Limit for the number of params
	PARAM_LIMIT	limittype;		// What params means (min,max,exact?)
	OpcodeHandle calllabel;		// Function call label (where to jump to to "call" the function!)
	FUNC		address;		// C function address
	void*		user;			// User value
	int			vars;			// Number of vars for varpage
	bool		baked;
};

// Conversion funcs
inline unsigned int MakeWord(int low, int high)
{
	return (low & 0xFFFF) | ((high & 0xFFFF) << 16);
}

inline unsigned int HighWord(int word)
{
	return (word & 0xFFFF0000) >> 16;
}

inline unsigned int LowWord(int word)
{
	return (word & 0xFFFF);
}

_END_SHILSCRIPT

#endif


