#include "Globals.h"
#include "State.h"
#include "Parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <math.h>

_USING_SHILSCRIPT

// ctor
State::State()
{
	m_outputter = 0;
	m_user		= 0;
	m_waitfunc	= 0;
	m_waituser  = 0;

	// Add 0 and 1 as constants in the constant table,
	// under id's 0 and 1 respectively :)
	AddConst(""); // This is as false as it can get :)
	AddConst("0");
	AddConst("1");
}


// dtor
State::~State()
{
	TermWaitFunc();
}


// Sets up a shilscript state
int State::Create(Inputter * inputter, Outputter * outputter, void * user)
{
	// check for invalid params
	if(!inputter || !outputter)
		return 0;
	
	// set up the state
	m_outputter	= outputter;	// set output method
	m_user		= user;			// set user value

	try
	{
		// Create a parser & start it off
		Parser P(this, inputter);
		P.Start();
	}
	catch(int unused)
	{
		unused=unused; // Ignore unreferenced local var stuff

		// Output an error and return failure
		Output("%s", m_err);
		return 0;
	}

	// Set up the current opcode state
	m_currcode = 0;

	// return success
	return 1;
}


// Adds a function to the state
int State::AddFunc(const char * name, FUNC func, int params, PARAM_LIMIT limittype, void * user)
{
	// TODO: Make safe against multiply defined funcs or something

	// Check against max
	if(m_funcs.Size() >= MAX_FUNCS)
		return 0;

	// Check for invalid params
	// MAINTENANCE: Change min/max for limit type
	if(!name || !func || params < 0 || limittype < PARAMS_MIN || limittype > PARAMS_EXACT)
		return 0;

	// convert the string to lowercase
	char lowername[MAX_ID_LEN];
	strncpy2(lowername, name, MAX_ID_LEN);
	strlwr2(lowername);

	// Create a structure for this function
	Function func_entry;
	func_entry.name			= SSString(lowername);
	func_entry.type			= FUNC_C;
	func_entry.params		= params;
	func_entry.limittype	= limittype;
	func_entry.address		= func;
	func_entry.calllabel	= 0;
	func_entry.user			= user;
	func_entry.baked		= true;
	m_funcs.PushBack(func_entry);

	// return success
	return 1;
}


// Macro for quick math opcodes :)
#define UNOP(__op, __expr) case (__op): { Value& top = m_stack.Top(); a = top.GetNum(); top = Value(__expr); break; }
#define BINOP(__op, __expr) case (__op): { b = m_stack.Top().GetNum(); m_stack.Pop(); Value& top = m_stack.Top(); a = top.GetNum(); top = Value(__expr); break; }

// Steps through the number of opcodes given.
// This is the very heart of the execution system ^_^
int State::Step(int steps)
{
	while(steps--)
	{
		// Check the wait function
		if(m_waitfunc)
		{
			// Call the wait func. If it returned 1 (done), end, otherwise don't do anything
			if((m_waitfunc)(this, m_waituser, WAIT_QUERY))
			{
				m_waitfunc = NULL;
				m_waituser = NULL;
			}
			else
				return 2; // Still continuing, just because we left early doesn't mean we're quitting
						  // TODO: Make a return val const for this
		}

		// Temp variables used in the opcodes
		Real a,b;

		// Throw an error if we hit the end when we shouldn't have :O :O :O
		if(m_currcode >= m_opcodes.Size())
		{
			RuntimeError(0, "Reached end of VM code without hitting OP_DONE");
			return 0;
		}

		// Get the current opcode
		Opcode op = m_opcodes[m_currcode];

		// Depending on the operation...
		switch(op.code)
		{
		// No operation (do nothing)
		case OP_NOOP:
			break;

		// Pop top of stack
		case OP_POP:
			m_stack.Pop();
			break;

		// Push constant onto stack
		case OP_PUSH:
			m_stack.Push(m_consts[op.val]);
			break;

		// Duplicate top of stack
		case OP_DUP:
			m_stack.Push(m_stack.Top());
			break;

		// Math operators
		UNOP(OP_NOT, a ? 0:1);
		UNOP(OP_NEG, -a);
		UNOP(OP_POS, (Real)fabs((Real)a));
		UNOP(OP_COMP, ~int(a));
		UNOP(OP_INC, a+1.0f);
		UNOP(OP_DEC, a-1.0f);
		BINOP(OP_ADD, a+b);
		BINOP(OP_SUB, a-b);
		BINOP(OP_MUL, a*b);
		BINOP(OP_SHL, int(a)<<int(b));
		BINOP(OP_SHR, int(a)>>int(b));
		BINOP(OP_AND, int(a) & int(b));
		BINOP(OP_OR, int(a) | int(b));
		BINOP(OP_XOR, int(a) ^ int(b));
		BINOP(OP_LAND, a && b);
		BINOP(OP_LOR, a || b);
		BINOP(OP_LT, a < b);
		BINOP(OP_GT, a > b);
		BINOP(OP_LE, a <= b);
		BINOP(OP_GE, a >= b);

		// Divide top 2 numbers
		case OP_DIV:
			b = m_stack.Top().GetNum();
			m_stack.Pop();
			a = m_stack.Top().GetNum();
			// Guard against divide by zero!
			if(b == 0)
			{
				RuntimeError(op.line, "Divide by zero");
				return 0;
			}
			m_stack.Top() = Value(a / b);
			break;

		// Modulus
		case OP_MOD:
			b = m_stack.Top().GetNum();
			m_stack.Pop();
			a = m_stack.Top().GetNum();
			// Guard against divide by zero!
			if(int(b) == 0)
			{
				RuntimeError(op.line, "Modulus divide by zero");
				return 0;
			}
			m_stack.Top() = Value(int(a) % int(b));
			break;

		// String concatenate
		case OP_CAT:
			{
				SSString s1, s2;
				s2 = m_stack.Top().GetStr();
				m_stack.Pop();
				s1 = m_stack.Top().GetStr();
				m_stack.Top() = Value(s1+s2);
			}
			break;

		// Equals (==)
		case OP_EQUAL:
			{
				Value v1, v2;
				v2 = m_stack.Top();
				m_stack.Pop();
				v1 = m_stack.Top();
				m_stack.Top() = Value(v1.IsEqual(v2));
			}
			break;

		// Not equal (!=)
		case OP_NE:
			{
				Value v1, v2;
				v2 = m_stack.Top();
				m_stack.Pop();
				v1 = m_stack.Top();
				m_stack.Top() = Value(!v1.IsEqual(v2));
			}
			break;

		// Function call
		case OP_CALL:
			{
				// Get the function referred to by this opcode
				int func_id = LowWord(op.val);
				int params = HighWord(op.val);

				if(InternalCall(op.line, func_id, params)) continue;
				else return 0;
			}
			break;

		// Return
		case OP_RETURN:
			{
				// If there aren't any labels to return to on the call stack, 
				if(m_callstack.Empty())
				{
					RuntimeError(op.line, "Callstack is empty!");
					return 0;
				}

				m_currcode = m_callstack.Top();

				m_callstack.Pop();

				// Delete the old top variable page and move back to the original
				m_pages.Pop();
			}
			goto dont_inc_currcode;
			//return 1;

		// Get a variable's value
		case OP_GETVAR:
			{
				if(op.val & SIGN_BIT)
					m_stack.Push(m_pages.Top()[op.val]);
				else
					m_stack.Push(m_vars[op.val]);
			}
			break;

		// Set a variable's value.
		// Leave its value on the top of the stack,
		// so you can do things like  if(a=b)
		case OP_SETVAR:
			{
				if(op.val & SIGN_BIT)
					m_pages.Top()[op.val] = m_stack.Top();
				else
					m_vars[op.val] = m_stack.Top();
			}
			break;

		// Jumps to a given label
		case OP_JMP:
			m_currcode = m_labels[op.val];
			goto dont_inc_currcode;
			//return 1;

		// Jumps to a given label if the top of the stack is true
		case OP_JT:
			a = m_stack.Top().GetNum();
			m_stack.Pop();
			if(a)
			{
				m_currcode = m_labels[op.val];
				continue;
			}
			else
				break;

		// Jumps to a given label if the top of the stack is FALSE
		case OP_JF:
			a = m_stack.Top().GetNum();
			m_stack.Pop();
			if(!a)
			{
				m_currcode = m_labels[op.val];
				continue;
			}
			else
				break;

		// We reached the end, terminate!
		case OP_DONE:
			MoveToEnd();
			return 0;

		default:
			{
			RuntimeError(op.line, "Unrecognized opcode");
			return 0;
			}
		}

		// Move on to the next one
		m_currcode++;

dont_inc_currcode:

		// Return success
		continue;
	}

	return 1;
}

// Writes output to the outputter.
int State::Output(const char * format, ...)
{
	// If there is no outputter, fail
	if(!m_outputter)
		return 0;

	// Print the parameters to a buffer via vsprintf()
	char szBuffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(szBuffer, format, args);
	va_end(args);
	strcat(szBuffer, "\r\n");

	// Write this output to the outputter
	return m_outputter->Write(szBuffer);
}


// Pops a value off the stack. *** This is dangerous - use only in bound C functions ***
Value State::Pop()
{
	if(m_stack.Empty())
		return Value("");

	Value val = m_stack.Top();
	m_stack.Pop();
	return val;
}


// Pushes a value onto the stack. *** This is dangerous - use only in bound C functions ***
int State::Push(Value val)
{
	m_stack.Push(val);
	return m_stack.Size();
}


// Prints a formatted error, and throws an exception
void State::Error(int line, const char * format, ...)
{
	// Print the parameters to a buffer via vsprintf()
	char szBuffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(szBuffer, format, args);
	va_end(args);

	// Now, create the full error string by adding the line #
	sprintf(m_err, "Error, line %d: %s", line, szBuffer);

	// Throw the error. We don't throw the char* ptr because it's out of scope
	// by the time we get to the catch, so it could have been overwritten,
	// which is a Bad Thing.
	throw(0);
}


// Prints a formatted runtim error
void State::RuntimeError(int line, const char * format, ...)
{
	MoveToEnd();

	// Print the parameters to a buffer via vsprintf()
	char szBuffer[256], szBuffer2[256];
	va_list args;
	va_start(args, format);
	vsprintf(szBuffer, format, args);
	va_end(args);

	// Now, create the full error string by adding the line #
	sprintf(szBuffer2, "Runtime Error, line %d: %s", line, szBuffer);

	Output("%s", szBuffer2);
}


// Sets the current wait function. Returns 1 if successful or 0 if not (i.e. if there's already a wait func)
int State::SetWaitFunc(WAITFUNC func, void * user)
{
	if(m_waitfunc)
		return 0;

	m_waitfunc = func;
	m_waituser = user;

	return 1;
}


// Calls a SHilScript function by name
int State::CallFunc(const char * name, int params)
{
	// Find the function that matches
	for(unsigned int i=0; i<m_funcs.Size(); i++)
		if(!stricmp2(m_funcs[i].name.c_str(), name))
		{
			// Rewind to the start (remove stack etc.)
			InternalRewind(params);

			// Move to the second-to-last opcode, so we return to the Done opcode
			m_currcode = m_opcodes.Size()-2;

			// Call the function
			return InternalCall(0, i, params);
		}

	return 0;
}


// Adds a constant to the constant table by value & returns its ID
int State::AddConst(SSString const_buf)
{
	// TODO: Redundancy checks?

	// Add it to the constant list
	Value val = Value(const_buf);
	val.CalcAllInfo();
	return (signed int)m_consts.PushBack(val); // TODO: Fix sign stuff
}


// Adds another slot to the variable table. It takes one parameter,
// a pointer to a current virtual index count. This is used if we're going
// to be using paged variables, i.e. for a function-level namespace.
// If num is NULL, then we just use the normal variable table, but otherwise
// we use the virtual count num points to.
int State::AddVar(int *num)
{
	if(num)
	{
		unsigned int id;

		if((unsigned)*num >= MAX_VARS)
			return -1;

		id = *num;
		(*num)++;
		id |= SIGN_BIT;
		return id;
	}
	else
	{
		if(m_vars.Size() >= MAX_VARS)
			return -1;

		return m_vars.PushBack(Value()); // & ~SIGN_BIT;
	}
}


// Adds a SHilScript function to the function table
int State::AddSHilScriptFunc(SSString name, int params, OpcodeHandle calllabel, bool baked)
{
	// Make sure we can add this func!
	if(m_funcs.Size() >= MAX_FUNCS)
		return -1;

	Function func;
	func.name		= name;
	func.type		= FUNC_SHILSCRIPT;
	func.params		= params;
	func.limittype	= PARAMS_EXACT;
	func.calllabel	= calllabel;
	func.baked		= baked;
	return m_funcs.PushBack(func);
}


// Registers another label for the VM code
int State::RegLabel()
{
	// Just create a new one and return its addy
	return m_labels.PushBack(0);
}

// Rewinds to the beginning of the script
void State::Rewind()
{
	InternalRewind(0);
}

// Rewinds to the beginning of the script.
// It keeps the number of specified values on the top of the stack.
void State::InternalRewind(int top)
{
	Stack<Value> m_temp;
	while(top-- && m_stack.Size())
	{
		m_temp.Push(m_stack.Top());
		m_stack.Pop();
	}

	TermWaitFunc();
	m_currcode = 0;
	m_stack.Clear();
	m_callstack.Clear();
	m_pages.Clear();

	m_stack = m_temp;
}


State& State::operator=(const State& other)
{
	TermWaitFunc();
	m_outputter				= other.m_outputter;
	m_user					= other.m_user;
	m_funcs					= other.m_funcs;
	m_callstack				= other.m_callstack;
	m_consts				= other.m_consts;
	m_opcodes				= other.m_opcodes;
	m_currcode				= other.m_currcode;
	m_vars					= other.m_vars;
	m_stack					= other.m_stack;
	m_labels				= other.m_labels;
	m_pages					= other.m_pages;
	m_waitfunc				= other.m_waitfunc;	// TODO: This isn't really safe >_<
	m_waituser				= other.m_waituser;

	return *this;
}

int State::InternalCall(int line, int func_id, int params)
{
	// Make sure it's valid
	if(func_id < 0 || func_id >= (signed)m_funcs.Size())
	{
		RuntimeError(line, "Invalid ID for OP_CALL: %d", func_id);
		return 0;
	}

	// Get a pointer to this function entry
	Function * func = &m_funcs[func_id];

	// If it's a C function, call it; otherwise jump to the function call label
	if(func->type == FUNC_C)
	{
		if(!((func->address)(this, params, func->user)))
		{
			RuntimeError(line, "C function '%s()' failed!", func->name.c_str());
			return 0;
		}

		m_currcode++;
		return 1;
	}
	else
	{
		// Make sure this function is defined
		if(!func->baked)
		{
			RuntimeError(line, "Call to prototyped but not defined function '%s()'", func->name.c_str());
			return 0;
		}

		// Allocate a var page for this func and set it as
		// the current variable page
		m_pages.Push();
		m_pages.Top().Reserve(func->vars);

		// Add the ID of the next code to the call stack
		// (so we can return to the next opcode when the function returns)
		m_callstack.Push(m_currcode+1);

		// Move to the label's call label (where to go when you call it, naturally :P)
		m_currcode = m_labels[func->calllabel];

		return 1;
	}
}

void State::TermWaitFunc()
{
	// Clean up any wait function
	if(m_waitfunc)
		(m_waitfunc)(this, m_waituser, WAIT_TERM);
}

void State::MoveToEnd()
{
	// Move to the last opcode, it should be a done
	m_currcode = m_opcodes.Size()-1;
}

bool State::Done()
{
	return(m_currcode == m_opcodes.Size()-1);
}
