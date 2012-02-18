// Globals.h - Global SHilScript schtuff

#ifndef _SHILSCRIPT_GLOBALS_H_
#define _SHILSCRIPT_GLOBALS_H_

#include <stdio.h>	// Standard stuff like sprintf
#include <string>	// std::string - TODO: Make own string class

// If we're using MSVC, disable some warnings that might interfere with usage
#ifdef _MSC_VER
	#pragma warning(disable: 4786)	// Disable "identifier truncated" warning from MSVC & STL
	#pragma warning(disable: 4355)	// 'this' used in base member initialize list
#endif //_MSC_VER

// Enable the use of a separate namespace for shilscript.
// Set this to zero to disable using the namespace.
#define _SHILSCRIPT_USE_SEPARATE_NAMESPACE 1

// If we're using a separate namespace for SHilScript, then set up the appropriate
// #define's so everything is enclosed in a namespace. If you want to use a separate
// namespace but "SHilScript" is too long, you can easily just change _SHILSCRIPT_NAMESPACE
// to something shorter (like "SHS") here.
#if(_SHILSCRIPT_USE_SEPARATE_NAMESPACE)
	#define _SHILSCRIPT_NAMESPACE SHilScript
	#define _SHILSCRIPT_FUNC(__func_name) _SHILSCRIPT_NAMESPACE :: __func_name
	#define _BEGIN_SHILSCRIPT namespace _SHILSCRIPT_NAMESPACE {
	#define _END_SHILSCRIPT };
	#define _USING_SHILSCRIPT using namespace _SHILSCRIPT_NAMESPACE ;
// Otherwise we're not using a separate namespace, so make the namespace-related macros
// do absolutely nothing :)
#else
	#define _SHILSCRIPT_NAMESPACE
	#define _SHILSCRIPT_FUNC(__func_name) __func_name
	#define _BEGIN_SHILSCRIPT
	#define _END_SHILSCRIPT
	#define _USING_SHILSCRIPT
#endif

// The calling convention for SHilScript callbacks
// Thanks to Mark "Nurgle" Collins for trying this out on Linux
// and finding this error...
#ifdef WIN32
	#define _SHILSCRIPT_CALL __stdcall
#else
	#define _SHILSCRIPT_CALL
#endif

// TODO: Centralize vector granularities here

_BEGIN_SHILSCRIPT

// Length settings
const unsigned int MAX_ID_LEN		= 256;		// This should be good enough. MSVC is 255 as well, and there are
												// hardly ever collisions (except STL :P)

const unsigned int MAX_CONST_LEN	= 4096;		// This is okay, but it's not the best. - TODO: Don't rely on static buffer

// You can change the precision you want here
// Thanks to Ian Overgard (aka FaceHat) for the name for this type
typedef double Real;

// Denotes an end of script
const int ENDOFSCRIPT = -1;

// Represents an un-checked-yet character
// for the lexing part of State
const int UNKNOWN = -2;

// Handle types.
typedef unsigned int OpcodeHandle;
typedef unsigned int LabelHandle;

// Object to read input from.
// ReadChar() returns either a character (0-255) or ENDOFSCRIPT (-1).
// ENDOFSCRIPT is equivalent to EOF.
class Inputter
{
public:
	Inputter() {}
	virtual ~Inputter() {}
	virtual int ReadChar()=0;
};


// Object to write output to
class Outputter
{
public:
	Outputter() {}
	virtual ~Outputter() {}
	virtual int Write(const char * line)=0;
};

// Use this as our string type
typedef std::basic_string<char> SSString;

int Hash(const char *key);
char * strlwr2(char * str);
char * strncpy2(char * dest, const char * src, size_t len);
int stricmp2( const char *string1, const char *string2 );

_END_SHILSCRIPT

#endif
