#include "Globals.h"
#include "State.h"
#include "Lexer.h"
#include "Tokens.h"
#include "Parser.h"

_USING_SHILSCRIPT

// Lexer constructor
Lexer::Lexer(State *S, Parser*P, Inputter * inputter)
{
	m_S		= S;
	m_P		= P;
	m_line	= 1;
	m_char	= UNKNOWN;
	m_char2	= UNKNOWN;
	m_inputter = inputter;
	InitKeywords();
}



// Deconstructor
Lexer::~Lexer()
{
	// Nothing
}



// Initializes the keyword table
void Lexer::InitKeywords()
{
	// For each keyword
	for(int kw=0; kw<KEYWORD_TOKEN_COUNT; kw++)
		// Find its matching token-name entry
		for(int tk=0; tk<TOKEN_NAME_COUNT; tk++)
			if(TOKEN_NAMES[tk].id == KEYWORD_TOKENS[kw])
			{
				// Create a keyword object & add it to the keyword list
				Keyword keyword;
				keyword.name = TOKEN_NAMES[tk].name;
				keyword.tk	 = KEYWORD_TOKENS[kw];
				keyword.hash = Hash(keyword.name.c_str());
				m_keywords.PushBack(keyword);
				break;
			}
}



// Reads in characters from the inputter, manages double character lookaheads
void Lexer::NextChar()
{
	// If there's no inputter (which shouldn't happen) return an EOF symbol
	if(!m_inputter)
	{
		m_char = m_char2 = ENDOFSCRIPT;
		return;
	}
	
	// If this is the first time we ran this
	// (we can tell because the second character's in an 'unknown' state...)
	if(m_char2 == UNKNOWN)
	{
		// Read in the first char
		m_char  = m_inputter->ReadChar();

		// If the first char was an EOS, then don't read in the second one;
		// assume it's EOS too. This would make a pretty lame script tho.
		if(m_char == ENDOFSCRIPT)
			m_char2 = ENDOFSCRIPT;
		else
			m_char2 = m_inputter->ReadChar();
	}
	// Otherwise
	else
	{
		// Shift the 2nd character forward to the first, and
		// then read into the second char.
		// Also make sure ENDOFSCRIPT is never returned more than once.

		// Cycle to the next character
		m_char	= m_char2;

		// If that was an EOS, then don't read another.
		// Otherwise, do.
		if(m_char == ENDOFSCRIPT)
			m_char2 = ENDOFSCRIPT;
		else
			m_char2	= m_inputter->ReadChar();
	}
}


// Reads in *ONE* token from the file.
Token Lexer::GetTok()
{
	while(1)
	{
		NextChar();

		switch(m_char)
		{
		// Extraneous whitespace
		case ' ': 
		case '\t':
		case '\r':
			// Skip
			break;

		// Newline
		case '\n':
			// Increase line #
			m_line++;
			break;

		// End-of-script
		case ENDOFSCRIPT:
			return Token(TK_ENDOFSCRIPT);

		// Plus
		case '+':
			// +=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_PLSE);
			}
			// ++
			else if(m_char2 == '+')
			{
				NextChar();
				return Token(TK_INC);
			}
			// +
			else
				return Token('+');

		// Minus
		case '-':
			// -=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_MNSE);
			}
			// --
			else if(m_char2 == '-')
			{
				NextChar();
				return Token(TK_DEC);
			}
			// -
			else
				return Token('-');

		// Multiply (star)
		case '*':
			// *=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_MULE);
			}
			// *
			else
				return Token('*');
		
		// Divide
		case '/':
			// /=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_DIVE);
			}
			// //
			else if(m_char2 == '/')
			{
				NextChar();
				while(m_char2 != '\n' && m_char2 != ENDOFSCRIPT)
					NextChar();
				break;
			}
			// /*
			else if(m_char2 == '*')
			{
				NextChar();
				EatSlashStarComment();
				break;
			}
			// /
			else
				return Token('/');

		// Shell-Style Comment (#)
		case '#':
			// #
			while(m_char2 != '\n' && m_char2 != ENDOFSCRIPT)
				NextChar();
			break;

		// Modulo
		case '%':
			// %=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_MODE);
			}
			// %
			else
				return Token('%');

		// String concat
		case '.':
			// .=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_CATE);
			}
			// .
			else
				return Token('.');

		// Less-than char 
		case '<':
			// <<...
			if(m_char2 == '<')
			{
				NextChar();

				// <<=
				if(m_char2 == '=')
				{
					NextChar();
					return Token(TK_SHLE);
				}
				// <<
				else
					return Token(TK_SHL);
			}
			// <=
			else if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_LE);
			}
			// <
			else
				return Token('<');

		// Greater-than char 
		case '>':
			// >>...
			if(m_char2 == '>')
			{
				NextChar();

				// >>=
				if(m_char2 == '=')
				{
					NextChar();
					return Token(TK_SHRE);
				}
				// >>
				else
					return Token(TK_SHR);
			}
			// >=
			else if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_GE);
			}
			// >
			else
				return Token('>');

		// And
		case '&':
			// &=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_ANDE);
			}
			// &&
			else if(m_char2 == '&')
			{
				NextChar();
				return Token(TK_LAND);
			}
			// &
			else
				return Token('&');

		// Or
		case '|':
			// |=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_ORE);
			}
			// ||
			else if(m_char2 == '|')
			{
				NextChar();
				return Token(TK_LOR);
			}
			// |
			else
				return Token('|');

		// Xor
		case '^':
			// ^=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_XORE);
			}
			// ^
			else
				return Token('^');

		// Not
		case '!':
			// !=
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_NE);
			}
			// !
			else
				return Token('!');

		// Equal
		case '=':
			// ==
			if(m_char2 == '=')
			{
				NextChar();
				return Token(TK_EQUAL);
			}
			// =
			else
				return Token('=');

		// String constant
		case '\"': case '\'':
			return Token(TK_CONST, ReadStrConst());

		// Zero-starting number constant (hex/oct)
		case '0':
			if(m_char2 == 'x')
			{
				NextChar();
				NextChar();
				return Token(TK_CONST, ReadHexConst());
			}
			else if(m_char2 >= '0' && m_char2 <= '7')
			{
				NextChar();
				return Token(TK_CONST, ReadOctConst());
			}
			else
				return Token(TK_CONST, ZERO_CONST_ID);

		// Other number-starting numerical constant (dec)
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			return Token(TK_CONST, ReadDecConst());

		// Other chars
		case '~': case '(':	case ')':
		case '?': case ':':	case ';':
		case '{': case '}':	case ',':
			return Token(m_char);

		// Something else
		default:
			// If it could be the start of an identifier or keyword
			if(isalpha(m_char) || m_char == '_')
				return ReadID();
			// Otherwise, we didn't recognize it, throw an error
			else
				m_S->Error(m_line, "Illegal character: '%c'", m_char);
		}
	}
}


// Finishes up stuff we didn't feel like doing directly in GetTok() for
// cleanness reasons :)
Token Lexer::GetTokWrapper()
{
	// Get the token
	Token tk = GetTok();

	// Modify its line #
	tk.line = m_line;

	// Return this new token
	return tk;
}


// Consumes a slash-star comment
void Lexer::EatSlashStarComment()
{
	// Loop until broken out of
	while(1)
	{
		// Move on to the next char
		NextChar();

		// If we found a */ combo
		if(m_char == '*' && m_char2 == '/')
		{
			// Move onto the next char & return
			NextChar();
			break;
		}
		// Otherwise, if we found a newline, make sure to increase the line count
		else if(m_char == '\n')
			m_line++;
		// And if we hit an EOF raise an error
		else if(m_char == ENDOFSCRIPT)
			m_S->Error(m_line, "Reached end of script while parsing /* comment");
	}
}



// Reads in a string constant and returns an index into the constant table
int Lexer::ReadStrConst()
{
	char buf[MAX_CONST_LEN];		// Buffer to store this constant's string equiv. in
	char* pbuf=buf;					// Current ptr to the next char to set
	char* end=buf+sizeof(buf);		// Just past the end of the buffer. If we reach this cause an error.

	// Discern the character to use for quoting
	int quotechar = m_char;

	// Loop until broken out of
	while(1)
	{
		// Move on to the next char
		NextChar();
		
		// If we saw the ending quote, break
		if(m_char == quotechar)
			break;
		// Otherwise, just copy this character to the buffer
		else
		{
			// If we hit a newline character, complain :)
			if(m_char == '\n' || m_char == '\r')
				m_S->Error(m_line, "Newline in constant");
			// Otherwise, if we hit a backslash...
			else if(m_char == '\\')
			{
				int c;					// Character this escape sequence evaluates to

				NextChar();

				// Determine what the sequence means
				switch(m_char)
				{
				// Normal, simple, one-character stuff
				case 'a':	c = '\a';	break;
				case 'b':	c = '\b';	break;
				case 'f':	c = '\f';	break;
				case 'n':	c = '\n';	break;
				case 'r':	c = '\r';	break;
				case 't':	c = '\t';	break;
				case 'v':	c = '\v';	break;
				case '?':	c = '\?';	break;
				case '\'':	c = '\'';	break;
				case '\"':	c = '\"';	break;
				case '\\':	c = '\\';	break;

				// Hex escape code, i.e. \xdd
				case 'x':
				{
					char temp[3];
					int i=0;

					for(i=0; i<2; i++)
					{
						if(isxdigit(m_char2))
						{
							temp[i] = m_char2;
							NextChar();
						}
						else
							break;
					}

					temp[i] = 0;

					if(!i)
						m_S->Error(m_line, "'\\xdd' escape sequence requires at least one digit following it");

					sscanf(temp, "%X", &c);
				}
				break;

				// Octal escape code, i.e. \ooo
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				{
					char temp[4];
					int i;

					for(i=0; i<3; i++)
					{
						temp[i] = m_char;
						if(m_char2 < '0' || m_char2 > '7')
						{
							i++;
							break;
						}
						else
							NextChar();
					}

					temp[i] = 0;

					sscanf(temp, "%o", &c);
				}
				break;

				// If we didn't find a match, complain :)
				default:	m_S->Error(m_line, "'%c': Unrecognized escape sequence", m_char);
				}

				// If the character is too big for 8 bits (this can happen with octal escape sequences),
				// cause an error
				if(c > 255)
					m_S->Error(m_line, "'%d': Too big for one character", c);

				// Move on
				/*if(c == 0) break;*/ // No need for more than one NULL
				*(pbuf++) = c;
			}
			else
			{
				// If we hit the ENDOFSCRIPT, complain
				if(m_char == ENDOFSCRIPT)
					m_S->Error(m_line, "Hit end-of-script while reading constant", m_char2);

				// Copy and move on
				/*if(m_char == 0) break;*/ // No need for more than one NULL
				*(pbuf++) = m_char;
			}
		}

		// If we hit the max size of the buffer, raise an error
		if(pbuf >= end)
			m_S->Error(m_line, "Constant too long (max is %d chars)", MAX_CONST_LEN-1);
	}

	// Null-terminate it
	//*pbuf=0;

	// Add it & return the ID
	return m_S->AddConst(SSString(buf, pbuf-buf));
}



// Reads in a decimal constant and returns an index into the constant table
int Lexer::ReadDecConst()
{
	char buf[MAX_CONST_LEN];		// Buffer to store this constant's string equiv. in
	char* pbuf=buf;					// Current ptr to the next char to set
	char* end=buf+sizeof(buf);		// Just past the end of the buffer. If we reach this cause an error.

	// TODO: e notation

	bool decimalalready=false;	// Have we encountered a decimal yet?

	// Loop until broken out of
	while(1)
	{
		// If this character's a decimal point
		if(m_char == '.')
			// If we already hit one
			if(decimalalready)
				// break
				break;
			// otherwise
			else
				// it's ok, just set the decimalalready flag
				decimalalready=true;

		// add this char to the buffer
		*(pbuf++) = m_char;

		// if we reached the end, cause an error
		if(pbuf == end)
			m_S->Error(m_line, "Constant too long (max is %d chars)", MAX_CONST_LEN-1);

		// Move on to the next char
		if((m_char2 >= '0' && m_char2 <= '9') || m_char2 == '.')
			NextChar();
		else
			break;
	}

	// Null-terminate it
	*pbuf=0;

	// Add it & return the ID
	return m_S->AddConst(buf);
}


// Reads in a hexadecimal constant
int Lexer::ReadHexConst()
{
	char buf[MAX_CONST_LEN];		// Buffer to store this constant's string equiv. in
	char* pbuf=buf;					// Current ptr to the next char to set
	char* end=buf+sizeof(buf);		// Just past the end of the buffer. If we reach this cause an error.

	// Loop until broken out of
	while(1)
	{
		// add this char to the buffer
		*(pbuf++) = m_char;

		// if we reached the end, cause an error
		if(pbuf == end)
			m_S->Error(m_line, "Constant too long (max is %d chars)", MAX_CONST_LEN-1);

		// Move on to the next char
		if(isxdigit(m_char2))
			NextChar();
		else
			break;
	}

	// Null-terminate it
	*pbuf=0;

	// Convert from hex to 
	int num;
	sscanf(buf, "%X", &num);
	sprintf(buf, "%u", num);

	// Add it & return the ID
	return m_S->AddConst(buf);
}


// Reads in a octal constant
int Lexer::ReadOctConst()
{
	char buf[MAX_CONST_LEN];		// Buffer to store this constant's string equiv. in
	char* pbuf=buf;					// Current ptr to the next char to set
	char* end=buf+sizeof(buf);		// Just past the end of the buffer. If we reach this cause an error.

	// Loop until broken out of
	while(1)
	{
		// add this char to the buffer
		*(pbuf++) = m_char;

		// if we reached the end, cause an error
		if(pbuf == end)
			m_S->Error(m_line, "Constant too long (max is %d chars)", MAX_CONST_LEN-1);

		// Move on to the next char
		if(m_char2 >= '0' && m_char2 <= '7')
			NextChar();
		else
			break;
	}

	// Null-terminate it
	*pbuf=0;

	// Convert from hex to 
	int num;
	sscanf(buf, "%o", &num);
	sprintf(buf, "%u", num);

	// Add it & return the ID
	return m_S->AddConst(buf);
}


// Reads an identifier from the input and returns its token.
Token Lexer::ReadID()
{
	char buf[MAX_ID_LEN];			// Buffer to hold the identifier's name in
	char * pbuf=buf;				// Ptr to current char in buf
	char * end=buf+sizeof(buf);		// Just past the end of buf

	// Build up the string
	while(1)
	{
		// Copy this character to the buffer
		*(pbuf++) = m_char;

		// Check if we overflowed it
		if(pbuf == end)
			m_S->Error(m_line, "Identifier too long (max is %d chars)", MAX_ID_LEN-1);

		// If the next character can't be in an identifier, break
		if(!isalpha(m_char2) && !isdigit(m_char2) && m_char2 != '_')
			break;

		// Otherwise advance to the next character
		NextChar();
	}

	// Null-terminate the buffer
	*pbuf=0;

	// Convert to lowercase (remove if you want case sensitivity)
	strlwr2(buf);

	// Check and see if it's a keyword token.

	// First, get the hash of the buffer in question
	int hash = Hash(buf);

	// Now, iterate through the keyword list.
	// If we find a keyword whose hash matches this hash, then do a real
	// string compare; if that works, thne return the token the keyword
	// corresponds to.
	for(unsigned int i=0; i<m_keywords.Size(); i++)
	{
		Keyword * kw = &m_keywords[i];
		if(kw->hash == hash)
			if(!strcmp(kw->name.c_str(), buf))
				return Token(kw->tk);
	}

	// Return an identifier token
	return Token(TK_ID, m_P->AddID(buf));
}

