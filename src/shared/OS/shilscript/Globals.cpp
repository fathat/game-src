// Globals.cpp - Global stuff definitions
#include "Globals.h"
#include <memory.h>

_USING_SHILSCRIPT

// "One-at-a-time hash" - found @ http://burtleburtle.net/bob/hash/doobs.html
int _SHILSCRIPT_FUNC(Hash(const char *key))
{
	int len = strlen(key);
	int hash, i;
	for(hash=0, i=0; i<len; ++i)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return (hash ^ 0x1337);
} 

// strlwr isn't ANSI.
// Thanks to Pouya Larjani (pouya@canada.com) for pointing this out.
char * _SHILSCRIPT_FUNC(strlwr2(char * str))
{
	int i = strlen(str);
	while(i--)
		str[i] = tolower(str[i]);
	return str;
}

// Safe string-copy. If the length is too short it includes the NULL.
char * _SHILSCRIPT_FUNC(strncpy2(char * dest, const char * src, size_t len))
{
	memset(dest, 0, len);
	strncpy(dest, src, len-1);

	return dest;
}

// Safe case-insensitive string compare. Returns 
int _SHILSCRIPT_FUNC(stricmp2( const char *dst, const char *src ))
{
	int f,l;
	do
	{
		f = tolower((unsigned char)*(dst++));
		l = tolower((unsigned char)*(src++));
	} while(f && (f == l));

	return (f-l);
}

