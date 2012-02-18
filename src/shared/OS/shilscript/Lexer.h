// Lexer.h - SHilScript lexer module

#ifndef _SHILSCRIPT_LEXER_H_
#define _SHILSCRIPT_LEXER_H_

#include "Tokens.h"
#include "Keywords.h"
#include "Vector.h"

_BEGIN_SHILSCRIPT

// Prototypes
class State;
class Parser;

// Lexing module. This module takes care of converting a stream of characters
// into a stream of tokens.
class Lexer
{
protected:

	int					m_line;			// Current line

	State *				m_S;			// Parent state
	Parser *			m_P;			// Parent parser
	Inputter *			m_inputter;		// Inputter object

	int					m_char;			// 1st lookahead character
	int					m_char2;		// 2nd lookahead character

	Vector<Keyword,10>	m_keywords;		// Keyword list

public:
	Lexer(State * S, Parser * P, Inputter * inputter);
	virtual ~Lexer();
	
private:
	void	InitKeywords();
	void	NextChar();
	void	EatSlashStarComment();
	int		ReadStrConst();
	int		ReadDecConst();
	int		ReadHexConst();
	int		ReadOctConst();
	Token	ReadID();
	Token	GetTok();

public:
	Token	GetTokWrapper();
};

_END_SHILSCRIPT

#endif


