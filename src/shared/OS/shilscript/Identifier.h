// Identifier.h - SHilScript identifier object

#ifndef _SHILSCRIPT_IDENTIFIER_H_
#define _SHILSCRIPT_IDENTIFIER_H_

_BEGIN_SHILSCRIPT

// Identifier types
enum
{
	ID_UNDEF=0,	// undefined
	ID_VAR,		// variable
	ID_FUNC,	// function
	ID_PROTO	// Prototyped (but not yet defined) func
};

// Identifier info
struct Identifier
{
public:
	SSString	name;			// Identifier name
	int			hash;			// Name hash
	int			type;			// Type (see enum above)
	int			scope;			// Scope this applies to
	int			id;				// Variable/Function ID

	Identifier() { name=""; type=ID_UNDEF; scope=0; id=0; }

	Identifier & operator=(const Identifier & other)
	{
		name	= other.name;
		hash	= other.hash;
		type	= other.type;
		scope	= other.scope;
		id		= other.id;
		return *this;
	}
};

_END_SHILSCRIPT

#endif


