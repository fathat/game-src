// Tokens.cpp - Token-related schtuff

#include "Globals.h"
#include "Tokens.h"

_USING_SHILSCRIPT

// Gets a token's name
SSString _SHILSCRIPT_FUNC(GetTokenName(int tk))
{
	// If it's a character
	if(tk < 256)
	{
		// Create a buffer containing the character in quotes
		// and return it
		char buf[2];
		sprintf(buf, "%c", tk);
		return SSString(buf);
	}
	else
	{
		// Otherwise search for this token & return its name
		for(int i=0; i<TOKEN_NAME_COUNT; i++)
			if(tk == TOKEN_NAMES[i].id)
				return TOKEN_NAMES[i].name;
	}

	// This should really NOT happen.
	return SSString("<<ERROR!!!>>");
}
