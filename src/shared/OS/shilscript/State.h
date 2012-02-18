// State.h - SHilScript VM state

#ifndef _SHILSCRIPT_STATE_H_
#define _SHILSCRIPT_STATE_H_

#include "Value.h"
#include "Opcodes.h"
#include "Tokens.h"
#include "Lexer.h"
#include "Identifier.h"
#include "Function.h"
#include "Vector.h"
#include "Stack.h"
#include "LLStack.h"

#include <string>

_BEGIN_SHILSCRIPT

// Maximum number of variables
const unsigned int MAX_VARS = (1<<31);		// Sign bit is used for paging, so we only have 31 bits available
const unsigned int SIGN_BIT = 0x80000000;	// Highest-order bit bitmask

// Prototype
class State;

// ID's of zero and one in the constant table
const int NULL_CONST_ID = 0;
const int ZERO_CONST_ID = 1;
const int ONE_CONST_ID = 2;

// Paged variable address space type
typedef Vector<Value, 25> VarPage;

// Wait callback func type
typedef int (_SHILSCRIPT_CALL *WAITFUNC)(State*,void*,int);
const int WAIT_QUERY	= 0;
const int WAIT_TERM		= 1;

// Encapsulation of a script engine (a VM "state")
class State
{
protected:

	friend class Lexer;
	friend class Parser;

	Outputter*				m_outputter;	// outputting object
	void*					m_user;			// User value
	Vector<Function,25>		m_funcs;		// Function table
	Stack<OpcodeHandle>		m_callstack;	// Call stack (where to return to!)
	Vector<Value,25>		m_consts;		// Constants
	Vector<Opcode,50>		m_opcodes;		// VM opcodes
	OpcodeHandle			m_currcode;		// Current opcode
	Vector<Value, 50>		m_vars;			// Variables
	Stack<Value>			m_stack;		// Stack for the VM
	Vector<OpcodeHandle, 25>m_labels;		// List of VM labels
	LLStack<VarPage>		m_pages;		// Variable pages
	WAITFUNC				m_waitfunc;		// Wait function
	void*					m_waituser;		// User value for wait function
	char					m_err[1024];	// Error text, as throwing a char* doesn't work well

public:
	State();
	virtual ~State();
	int Create(Inputter* inputter, Outputter* outputter, void* user);
	int AddFunc(const char * name, FUNC func, int params, PARAM_LIMIT limittype, void* user);
	int Step(int steps);
	int Output(const char * format, ...);
	int Push(Value val);
	Value Pop();
	void*	GetUserVal()			{ return m_user; }
	void	SetUserVal(void* user)	{ m_user = user; }

	Outputter*	GetOutputter()						{ return m_outputter; }
	int			SetOutputter(Outputter* outputter)	{ if(outputter) {m_outputter = outputter; return 1;} else return 0; }

	void	Rewind(); // Rewinds to beginning of script
	State&  operator=(const State& other);

	int		SetWaitFunc(WAITFUNC func, void * user);
	int		CallFunc(const char * name, int params);
	bool	Done();
protected:
	void	InternalRewind(int top);
	void	RuntimeError(int line, const char * format, ...);
	void	Error(int line, const char * format, ...);
	inline void	SetLabel(LabelHandle label, OpcodeHandle target) { m_labels[label] = target; }
	int		AddConst(SSString const_buf);
	int		AddVar(int *num);
	int		AddSHilScriptFunc(SSString name, int params, OpcodeHandle calllabel, bool baked);
	int		RegLabel();
	int		InternalCall(int line, int func_id, int params);
	void	TermWaitFunc();
	void	MoveToEnd();
};

_END_SHILSCRIPT

#endif


