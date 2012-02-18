// Parser.cpp - Parsing

#include "Globals.h"
#include "Parser.h"
#include "State.h"
#include "Operators.h"

_USING_SHILSCRIPT

// Constructor
Parser::Parser(State * S, Inputter *inputter) : m_S(S), m_L(S, this, inputter)
{
	m_scope = 0;
	m_infunc = false;
	m_funcvars = 0;

	// Add identifiers for each of the C functions in the state's function table
	for(int i=0; i<(signed)m_S->m_funcs.Size(); i++)
	{
		// Create an identifier for this function
		int func_id = AddID(m_S->m_funcs[i].name.c_str());

		// Get a pointer to it
		Identifier * id = &m_ids[func_id];

		// Fill it in - type is function, id is this function's ID
		id->type = ID_FUNC;
		id->id	 = i;
	}
}


// Destructor
Parser::~Parser()
{
}


// Reads the next token(s) from the file, manages double lookaheads
void Parser::NextTok()
{
	// If this is the first time we've run this
	if(m_lookahead2.type == UNKNOWN)
	{
		// Read in the values for both characters
		m_lookahead	 = m_L.GetTokWrapper();
		m_lookahead2 = m_L.GetTokWrapper();
	}
	// Otherwise
	else
	{
		// Cycle the 2nd lookahead forward to the 1st slot
		// and read a new one into the 2nd slot
		m_lookahead	 = m_lookahead2;
		m_lookahead2 = m_L.GetTokWrapper();
	}
}


// Verifies that the next token in the stream is of the specified type. If it
// is not, it creates an "Expected 'blah'" error.
void Parser::Match(int tk)
{
	// Make sure we got this token. If not, cause an error.
	if(m_lookahead.type != tk)
		m_S->Error(m_lookahead.line, "Expected '%s', got '%s'", GetTokenName(tk).c_str(), GetTokenName(m_lookahead.type).c_str());
	
	NextTok();
}


// Adds an opcode to the output opcode list
int Parser::AddOpcode(int id, int line, int val)
{
	//printf("%04u\t%s\t%08X\n", line, OPCODE_NAMES[id].c_str(), val);

	// Add it to the end of the state's opcode list
	return m_S->m_opcodes.PushBack(Opcode(id,line,val));
}


// Increases the scope
void Parser::ScopeUp()
{
	// Increase the scope counter
	m_scope++;
}


// Decreases the scope
void Parser::ScopeDown()
{
	// Neutralize any id's from a higher scope
	for(int i=0; i<(signed)m_ids.Size(); i++)
		if(m_ids[i].scope >= m_scope)
		{
			m_ids[i].type	= ID_UNDEF;
			m_ids[i].id		= 0;
			m_ids[i].scope	= 0;
		}

	// Decrease the scope counter
	m_scope--;
}


// Adds an ID name to the ID name table by value & returns its ID
int Parser::AddID(const char * buf)
{
	// Get the hash of the buffer
	int hash = Hash(buf);

	// Search for possible matches
	int size = m_ids.Size();
	for(int i=0; i<size; i++)
		if(m_ids[i].hash == hash)
			if(!strcmp(m_ids[i].name.c_str(), buf))
				return i;

	// If we got this far, there were no previous matches.
	// Construct a new ID
	Identifier ID;
	ID.name		= SSString(buf);
	ID.hash		= hash;
	ID.type		= ID_UNDEF;

	// Add it to the ID vector
	return m_ids.PushBack(ID);
}


// Starts off parsing
void Parser::Start()
{
	// Start off the lexer
	NextTok();

	// Read statements until we reach the end of the script
	while(m_lookahead.type != TK_ENDOFSCRIPT)
		Stmt();

	// Add the Done opcode
	AddOpcode(OP_DONE, m_lookahead.line, 0);

	// Verify functions
	for(unsigned int i=0; i<m_S->m_funcs.Size(); i++)
	{
		Function * func = &m_S->m_funcs[i];
		if(!func->baked)
			m_S->Error(m_lookahead.line, "Function '%s()' prototyped but not defined", func->name.c_str());
	}
}



// Handles parsing a statement
void Parser::Stmt()
{
	// Figure out what type of statement this is
	switch(m_lookahead.type)
	{
	case TK_IF:			IfStmt();		break;
	case TK_WHILE:		WhileStmt();	break;
	case TK_DO:			DoStmt();		break;
	case TK_FOR:		ForStmt();		break;
	case TK_BREAK:		BreakStmt();	break;
	case TK_CONTINUE:	ContinueStmt();	break;
	case TK_VAR:		DeclStmt();		break;
	case TK_FUNC:		FuncStmt();		break;
	case TK_RETURN:		ReturnStmt();	break;
	case TK_PROTOTYPE:	PrototypeStmt();break;
	case TK_EXIT:		ExitStmt();		break;
	case '{':			CompoundStmt();	break;
	case ';':			Match(';');		break;

	// If it wasn't anything ELSE, it has to have been an expression stmt
	default: ExprStmt();	break;
	}
}



// Parses an IF statement.
void Parser::IfStmt()
{
	// Create labels
	LabelHandle	elselabel = m_S->RegLabel(),
				donelabel = m_S->RegLabel();

	// Match the initial if keyword and the parentheses
	Match(TK_IF);
	Match('(');

	// Match the expression
	Expr();

	// Match the ending parentheses
	Match(')');

	// Add the JF opcode
	AddOpcode(OP_JF, m_lookahead.line, (int)elselabel);

	// Add the controlled statement
	ScopeUp();
	Stmt();
	ScopeDown();

	// Jump to the end
	AddOpcode(OP_JMP, m_lookahead.line, (int)donelabel);

	// Add the "else" label target
	m_S->SetLabel(elselabel, AddOpcode(OP_NOOP, m_lookahead.line, 0));

	// If we got an else
	if(m_lookahead.type == TK_ELSE)
	{
		// Match the else keyword
		Match(TK_ELSE);

		// Read in the controlled statement
		Stmt();
	}

	// Add the "done" label target
	m_S->SetLabel(donelabel, AddOpcode(OP_NOOP, m_lookahead.line, 0));
}


// Parses a While statement
void Parser::WhileStmt()
{
	// Create labels
	LabelHandle	contlabel  = m_S->RegLabel(),
				breaklabel = m_S->RegLabel();

	int topline = m_lookahead.line;

	// Add the continue/break labels to the label stacks
	m_breakstack.Push(breaklabel);
	m_contstack.Push(contlabel);

	// Match the initial while keyword and parentheses
	Match(TK_WHILE);
	Match('(');

	// Add the first NOOP, the continue labeled jump target
	m_S->SetLabel(contlabel, AddOpcode(OP_NOOP, topline, 0));

	// Add the conditional expression
	Expr();

	// Match the ending parentheses
	Match(')');

	// Add the JF opcode
	AddOpcode(OP_JF, topline, (int)breaklabel);

	// Add the controlled statement
	ScopeUp();
	Stmt();
	ScopeDown();

	// Jump to the continue label
	AddOpcode(OP_JMP, topline, (int)contlabel);

	// Add the last NOOP, the break labeled jump target
	m_S->SetLabel(breaklabel, AddOpcode(OP_NOOP, topline, 0));

	// Pop off our continue/break labels
	m_breakstack.Pop();
	m_contstack.Pop();
}



// Parses a Do statement
void Parser::DoStmt()
{
	// Create labels
	LabelHandle	redolabel	= m_S->RegLabel(),
				contlabel	= m_S->RegLabel(),
				breaklabel	= m_S->RegLabel();

	int topline = m_lookahead.line,
		bottomline,
		lastline;

	// Add the continue/break labels to the label stacks
	m_breakstack.Push(breaklabel);
	m_contstack.Push(contlabel);

	// Match the initial do keyword
	Match(TK_DO);

	// Add the first NOOP, the redo label
	m_S->SetLabel(redolabel, AddOpcode(OP_NOOP, topline, 0));

	// Add the controlled statement
	ScopeUp();
	Stmt();
	ScopeDown();

	// Match the while keyword
	bottomline = m_lookahead.line;
	Match(TK_WHILE);
	Match('(');

	// Add the continue label
	m_S->SetLabel(contlabel, AddOpcode(OP_NOOP, bottomline, 0));

	// Add the conditional expression
	Expr();

	// Add the closing parentheses & semicolon
	lastline = m_lookahead.line;
	Match(')');
	Match(';');
	
	// Add the last two opcodes, JT redo and break
	AddOpcode(OP_JT, lastline, (int)redolabel);
	m_S->SetLabel(breaklabel, AddOpcode(OP_NOOP, lastline, 0));
	
	// Pop off our continue/break labels
	m_breakstack.Pop();
	m_contstack.Pop();
}


// Handles the for statement
void Parser::ForStmt()
{
	// Create labels
	LabelHandle	testlabel	= m_S->RegLabel(),
				alabel		= m_S->RegLabel(),
				blabel		= m_S->RegLabel(),
				contlabel	= m_S->RegLabel(),
				breaklabel	= m_S->RegLabel();

	int topline = m_lookahead.line;

	// Match the for keyword & first parentheses
	Match(TK_FOR);
	Match('(');

	// Crank up the scope :)
	ScopeUp();

	// Add the opcodes for the first statement
	Stmt();
	
	// Add the test target
	m_S->SetLabel(testlabel, AddOpcode(OP_NOOP, topline, 0));

	// Add the continue/break labels to the label stacks
	// (do it here so people can't call continue/break from the init-stmt)
	m_breakstack.Push(breaklabel);
	m_contstack.Push(contlabel);

	// If there's a test expression, add it
	if(m_lookahead.type != ';')
	{
		// Match the expression and add the JF opcode
		Expr();
		AddOpcode(OP_JF, topline, (int)breaklabel);
	}

	Match(';');

	// Jump to B from here
	AddOpcode(OP_JMP, topline, (int)blabel);

	// Add the A label jump target
	m_S->SetLabel(alabel, AddOpcode(OP_NOOP, topline, 0));

	// Read in the increment expressions
	while(m_lookahead.type != ')')
	{
		Expr();
		AddOpcode(OP_POP, topline, 0);

		if(m_lookahead.type == ')')
			break;
		else
			Match(',');
	}

	// Match the ending parentheses
	Match(')');

	// Add the JMP test statement & the B label
	AddOpcode(OP_JMP, topline, (int)testlabel);
	m_S->SetLabel(blabel, AddOpcode(OP_NOOP, topline, 0));

	// Add the controlled statement
	ScopeUp();
	Stmt();
	ScopeDown();

	// Add the last two opcodes
	m_S->SetLabel(contlabel, AddOpcode(OP_JMP, m_lookahead.line, (int)alabel));
	m_S->SetLabel(breaklabel, AddOpcode(OP_NOOP, m_lookahead.line, 0));

	// Crank down the scope, again
	ScopeDown();

	// Pop off our continue/break labels
	m_breakstack.Pop();
	m_contstack.Pop();
}

// Handles the break keyword
void Parser::BreakStmt()
{
	int line = m_lookahead.line;

	// Match the break statement
	Match(TK_BREAK);
	Match(';');

	// Make sure it's not illegal
	if(!m_breakstack.Size())
		m_S->Error(line, "Illegal break");

	// Otherwise, add a JMP statement to go
	// to the break label!
	AddOpcode(OP_JMP, line, (int)m_breakstack.Top());
}


// Handles the continue keyword
void Parser::ContinueStmt()
{
	int line = m_lookahead.line;

	// Match the break statement
	Match(TK_CONTINUE);
	Match(';');

	// Make sure it's not illegal
	if(!m_contstack.Size())
		m_S->Error(line, "Illegal continue");

	// Otherwise, add a JMP statement to go
	// to the continue label!
	AddOpcode(OP_JMP, line, (int)m_contstack.Top());
}


void Parser::DeclStmt()
{
	// Make sure we got the var keyword
	Match(TK_VAR);

	// We assume at least one var will be declared...
	while(1)
	{
		// Make sure the next token is an identifier
		if(m_lookahead.type != TK_ID)
			// Match an ID just to cause an error
			Match(TK_ID);

		// Get the identifier info for this id name
		int la_id = m_lookahead.id;
		Identifier * id = &m_ids[la_id];

		// Get its line no.
		int line = m_lookahead.line;

		// If it's not undefined, raise an error.
		// We don't want to redefine variables!
		if(id->type != ID_UNDEF)
			m_S->Error(line, "'%s' already declared", id->name.c_str());

		// Read over the ID token
		Match(TK_ID);

		// Was there an equals sign?
		// If so, read in a value (like, var a=5;)
		if(m_lookahead.type == '=')
		{
			// Match the equals sign and read in an expression
			Match('=');
			Expr();
		}
		// Otherwise there wasn't one, so just push null onto the
		// stack as the default (as if it were var a="";)
		else
			AddOpcode(OP_PUSH, line, NULL_CONST_ID);

		// Get the ptr to the identifier back, because the vector could have been
		// reallocated by now
		id = &m_ids[la_id];

		// Set up this new variable
		id->type	= ID_VAR;
		id->scope	= m_scope;
		id->id		= m_S->AddVar(m_infunc ? &m_funcvars : 0);	// Being paged or not depends if we're in a function...
		if(id->id == -1)
			m_S->Error(line, "Ran out of variables!");

		// Set its value
		AddOpcode(OP_SETVAR, line, id->id);

		// If the next token is a semicolon, break.
		// Otherwise, read in a comma
		if(m_lookahead.type == ';')
		{
			Match(';');
			break;
		}
		else
			Match(',');
	}
}



void Parser::FuncStmt()
{
	// Labels
	LabelHandle skiplabel = m_S->RegLabel(),
				calllabel = m_S->RegLabel();

	int startline = m_lookahead.line;

	// Make sure we're in global scope
	if(m_scope != 0)
		m_S->Error(m_lookahead.line, "Functions must have global scope");

	// Reset the function var count
	m_funcvars = 0;

	// Now we can scope up
	ScopeUp();

	// Match the function keyword
	Match(TK_FUNC);

	// If the next token isn't an ID (as it should be the function's name)
	// deliberately Match() an ID so it will raise an error
	if(m_lookahead.type != TK_ID)
		Match(TK_ID);

	// Get the identifier info for the lookahead token (the func. name)
	int la_id = m_lookahead.id;
	Identifier * id = &m_ids[la_id];

	// Make sure it's not already declared
	if(id->type != ID_UNDEF && id->type != ID_PROTO)
		m_S->Error(m_lookahead.line, "'%s' already declared", id->name.c_str());

	// Save whether or not this func has already been prototyped
	bool prototyped = (id->type == ID_PROTO);

	// Since it's not declared, declare it as a function
	id->type	= ID_FUNC;
	if(!prototyped)
	{
		id->scope	= 0;
		id->id		= 0; // We'll set this down a bit farther
	}
	
	SSString func_name = id->name;

	// Match the ID token
	Match(TK_ID);

	// Read in the beginning parentheses of the parameter list
	Match('(');

	// Add the jump opcode so the function isn't executed where it's declared
	AddOpcode(OP_JMP, startline, (int)skiplabel);

	// Add the jump target for function calls
	m_S->SetLabel(calllabel, AddOpcode(OP_NOOP, startline, 0));

	int param_count=0; // # of parameters

	// If there are any params
	if(m_lookahead.type != ')')
	{	
		// Read in the params.
		Stack<int> param_ids;
		Stack<int> param_lines;

		// Get all the parameter variable names and create variables for them
		while(1)
		{
			// If there aren't any params left, break
			if(m_lookahead.type == ')')
				break;

			// Make sure we hit an identifier
			if(m_lookahead.type != TK_ID)
				m_S->Error(m_lookahead.line, "Expected identifier");
			
			// Get the identifier info for this name
			int la_id2 = m_lookahead.id;
			Identifier * id2 = &m_ids[la_id2];

			// Make sure it's not already declared
			if(id2->type != ID_UNDEF)
				m_S->Error(m_lookahead.line, "'%s' already declared", id2->name.c_str());

			// Declare it
			id2->type	= ID_VAR;
			id2->scope	= m_scope;
			id2->id		= m_S->AddVar(&m_funcvars);	// We've gotta be paged if we're in a function already
			if(id2->id == -1)
				m_S->Error(m_lookahead.line, "Ran out of variables!");

			// Add it to the stack
			param_ids.Push(id2->id);
			param_lines.Push(m_lookahead.line);

			// Match the ID token
			Match(TK_ID);

			// If we're not going to hit the end, we'd better hit a comma
			if(m_lookahead.type != ')')
				Match(',');
		}

		// Save the # of parameters
		param_count = param_ids.Size();

		// Now add the code to set all these parameter vars
		while(param_ids.Size())
		{
			AddOpcode(OP_SETVAR, param_lines.Top(), param_ids.Top());
			AddOpcode(OP_POP, param_lines.Top(), 0);
			param_ids.Pop();
			param_lines.Pop();
		}
	}

	Match(')');

	// Add an entry to the function table
	int func_id;
	if(!prototyped)
	{
		func_id = m_S->AddSHilScriptFunc(func_name, param_count, calllabel, true);
		if(func_id == -1)
			m_S->Error(startline, "Can't add '%s()' - Reached %u-function limit", func_name.c_str(), MAX_FUNCS);

		// Add this info to the ID entry
		id = &m_ids[la_id];
		id->id = func_id;
	}
	else
	{
		// Add this info to the ID entry
		id = &m_ids[la_id];

		func_id = id->id;
		m_S->m_funcs[func_id].calllabel = calllabel;
		m_S->m_funcs[func_id].baked = true;

		// Make sure the param counts match
		if(m_S->m_funcs[func_id].params != param_count)
			m_S->Error(startline, "Parameters for '%s()' definition don't match prototype", func_name.c_str());
	}

	// Now for the main compound block
	m_infunc = true;
	CompoundStmt();
	m_infunc = false;

	// Add the opcode to push a blank value onto the stack (if a return wasn't called)
	AddOpcode(OP_PUSH, startline, NULL_CONST_ID);

	// Add the opcode to return to the call point
	AddOpcode(OP_RETURN, startline, 0);

	// Scope down
	ScopeDown();

	// Add the skip label target
	m_S->SetLabel(skiplabel, AddOpcode(OP_NOOP, startline, 0));

	// Save the number of vars to the function info
	m_S->m_funcs[func_id].vars = m_funcvars;
	m_funcvars = 0;
}


void Parser::PrototypeStmt()
{
	// We can only do this at zero scope
	if(m_scope != 0)
		m_S->Error(m_lookahead.line, "Functions must have global scope");

	int startline = m_lookahead.line;

	// Match the prototype keyword
	Match(TK_PROTOTYPE);

	// Get the ID of the identifier
	int la_id = m_lookahead.id;
	Identifier * id = &m_ids[la_id];

	// Make sure it's not already declared
	if(id->type != ID_UNDEF)
		m_S->Error(m_lookahead.line, "'%s' already declared", id->name.c_str());

	// Declare it as a prototyped function
	id->type	= ID_PROTO;
	id->scope	= 0;
	id->id		= 0; // We'll set this down a bit farther
	SSString func_name = id->name;

	int param_count=0;

	// Match the ID token and the first parantheses
	Match(TK_ID);
	Match('(');

	if(m_lookahead.type != ')')
		while(1)
		{
			// If it's not the correct type, call match to flag it early
			if(m_lookahead.type != TK_ID)
				Match(TK_ID);

			// Get info about it
			Identifier * id2 = &m_ids[m_lookahead.id];
			if(id2->type != ID_UNDEF)
				m_S->Error(m_lookahead.line, "'%s' already declared", id2->name.c_str());

			Match(TK_ID);
			param_count++;
			if(m_lookahead.type != ')') Match(',');
			else break;
		}

	// Match the ending parantheses and semicolon
	Match(')');
	Match(';');

	// Create is as an unbaked (i.e. undefined) function
	int func_id = m_S->AddSHilScriptFunc(func_name, param_count, 0, false);
	if(func_id == -1)
		m_S->Error(startline, "Can't add '%s()' - Reached %u-function limit", func_name.c_str(), MAX_FUNCS);

	// Save the func ID to the identifier's info
	id = &m_ids[la_id];
	id->id = func_id;
}



void Parser::ExitStmt()
{
	// Pretty simple here. The exit statement just ends script execution :)
	AddOpcode(OP_DONE, m_lookahead.line, 0);
	Match(TK_EXIT);
	Match(';');
}


void Parser::ReturnStmt()
{
	// Make sure we can handle a return statement
	if(!m_infunc)
		m_S->Error(m_lookahead.line, "Illegal return");

	// Match the return statement
	Match(TK_RETURN);

	// If this is a void return statement, push a null value;
	// otherwise parse an expression and leave its value
	// on top of the stack
	if(m_lookahead.type == ';')
		AddOpcode(OP_PUSH, m_lookahead.line, NULL_CONST_ID);
	else
		Expr();

	// Add the opcode to jump to the return point
	AddOpcode(OP_RETURN, m_lookahead.line, 0);

	// Match the ending semicolon
	Match(';');
}


void Parser::CompoundStmt()
{
	// Match the beginning brace
	Match('{');

	// Increase the scope
	ScopeUp();

	// While we haven't reached the end of this block
	while(m_lookahead.type != '}')
	{
		// Read in a statement
		Stmt();

		// If we hit the end-of-script, cause an error
		if(m_lookahead.type == TK_ENDOFSCRIPT)
			m_S->Error(m_lookahead.line, "Unexpected end of script");
	}

	// Decrease the scope
	ScopeDown();

	// Match the end brace
	Match('}');
}



void Parser::ExprStmt()
{
	// Execute an expression
	Expr();

	// Add a "pop" opcode.
	// Because all arithmetic instructions
	// return a value, then there'll be a value on the stack
	// after the expr part of an expr-stmt, so we have to
	// pop it, or else our stack'll just start overflowing,
	// not to mention it behaving incorrectly.
	AddOpcode(OP_POP, m_lookahead.line, 0);

	// Match
	Match(';');
}



void Parser::Expr()
{
	// The first possibility for an expression is an assignment expression.
	AssignExpr();
}



void Parser::AssignExpr()
{
	// Determine if the second-to-next token is an assignment operator.
	// Also determine the token to Match() and the opcode to use for
	// applying this assignment.
	bool isassign=false;
	int tk,opcode;
	if(m_lookahead2.type == '=')
	{
		tk='=';
		opcode=0;
		isassign=true;
	}
	else
		for(int row=0; row<ASSIGN_ROWS; row++)
			if(ASSIGN_OPS[row].tk == m_lookahead2.type)
			{
				tk			= m_lookahead2.type;
				opcode		= ASSIGN_OPS[row].opcode;
				isassign	=true;
				break;
			}


	// If it was an assignment operator
	if(isassign)
	{
		// Make sure the next token is an identifier. If it's not,
		// we could cause trouble getting its identifier info :P
		if(m_lookahead.type != TK_ID)
			m_S->Error(m_lookahead.line, "Expected identifier");

		// Get this var's ID
		int var = GetVarID();

		// Get the token's line #
		int line = m_lookahead.line;

		// Match the identifier token & the equals sign
		Match(TK_ID);
		Match(tk);

		// If necessary, add the opcodes for reading in this var's current value
		if(opcode)
			AddOpcode(OP_GETVAR, line, var);

		// Match another assignment expression. This is so
		// you can do a=b=c=d;
		AssignExpr();

		// Apply any operation to the original var + the result if necessary
		// (for +=, *=, etc.)
		if(opcode)
			AddOpcode(opcode, line, var);

		// Set the var
		AddOpcode(OP_SETVAR, line, var);
	}
	// Otherwise drop thru to an addition expr.
	else
		BinaryExpr(0);
}




// Handles generic binary operators ^_^
void Parser::BinaryExpr(int row)
{	
	// If we're past the last row, just call the unary
	// operator handler
	if(row == BINARY_ROWS)
	{
		UnaryExpr();
		return;
	}

	// Read in the next-higher-precedence expr
	BinaryExpr(row+1);

REDO:

	// For each column in the binary operator table's current row
	for(int col=0; col<BINARY_COLS; col++)
	{
		// Get the current operator at this row, column
		const BinaryOp * op = &BINARY_OPS[row][col];

		// If this was a "zeroed out" operator entry, it means that
		// there aren't any more operator entries in this column
		if(!op->tk)
			break;

		// If the lookahead token is the operator token
		if(m_lookahead.type == op->tk)
		{
			// Get the current line #
			int line = m_lookahead.line;

			// Match the operator token and the next-highest-precedence expr
			Match(op->tk);
			BinaryExpr(row+1);

			// Add the opcode
			AddOpcode(op->opcode, line, 0);

			// Goto the REDO label to start over this row
			goto REDO;
		}
	}
}




// A unary operator expression (!, ~, etc)
void Parser::UnaryExpr()
{
	// Get the line # of the lookahead
	int line = m_lookahead.line;

	// Choose the opcode depending on the unary
	// operator we encounter. If we don't hit any,
	// just drop down to a basic-expr
	int op=0;
	switch(m_lookahead.type)
	{
	case '!':	op = OP_NOT;	break;
	case '+':	op = OP_POS;	break;
	case '-':	op = OP_NEG;	break;
	case '~':	op = OP_COMP;	break;

	// Prefix increment operator
	case TK_INC:
		{
			Match(TK_INC);
			int var = GetVarID();
			Match(TK_ID);
			AddOpcode(OP_GETVAR, line, var);
			AddOpcode(OP_INC,	 line, 0);
			AddOpcode(OP_SETVAR, line, var);
		}
		return;

	// Prefix decrement operator
	case TK_DEC:
		{
			Match(TK_DEC);
			int var = GetVarID();
			Match(TK_ID);
			AddOpcode(OP_GETVAR, line, var);
			AddOpcode(OP_DEC,	 line, 0);
			AddOpcode(OP_SETVAR, line, var);
		}
		return;

	default:

		if(m_lookahead.type == TK_ID)
		{
			// Postfix increment operator
			if(m_lookahead2.type == TK_INC)
			{
				// Get info about the var
				int var = GetVarID();

				// Match the ID and operator
				Match(TK_ID);
				Match(TK_INC);

				// Push var's value onto the stack, duplicate it,
				// increment it, copy it back to the var, and pop off the top,
				// leaving the original value there.
				AddOpcode(OP_GETVAR, line, var);
				AddOpcode(OP_DUP,    line, 0);
				AddOpcode(OP_INC,    line, 0);
				AddOpcode(OP_SETVAR, line, var);
				AddOpcode(OP_POP,    line, 0);

				return;
			}

			// Postfix decrement operator
			else if(m_lookahead2.type == TK_DEC)
			{
				// Get info about the var
				int var = GetVarID();

				// Match the ID and operator
				Match(TK_ID);
				Match(TK_DEC);

				// Push var's value onto the stack, duplicate it,
				// decrement it, copy it back to the var, and pop off the top,
				// leaving the original value there.
				AddOpcode(OP_GETVAR, line, var);
				AddOpcode(OP_DUP,    line, 0);
				AddOpcode(OP_DEC,    line, 0);
				AddOpcode(OP_SETVAR, line, var);
				AddOpcode(OP_POP,    line, 0);

				return;
			}
		}

		BasicExpr();
		return;
	}

	// Match the operator we found and add the opcode
	Match(m_lookahead.type);
	UnaryExpr();
	AddOpcode(op, line, 0);
}



// A basic, lowest-possible-level expression (high precedence)
void Parser::BasicExpr()
{
	// If the next token's an ID, read it in
	// as a variable
	if(m_lookahead.type == TK_ID)
	{
		// If the next token is a '(' (i.e., it should be a function)
		if(m_lookahead2.type == '(')
		{
			int line = m_lookahead.line;

			// Get a pointer to this identifier
			Identifier * id = &m_ids[m_lookahead.id];

			// Make sure it's a function!
			if(id->type != ID_FUNC && id->type != ID_PROTO)
				m_S->Error(m_lookahead.line, "'%s' is not a function", id->name.c_str());

			// Get the function id
			int func_id = id->id;

			// Match the ID token and the leading parentheses
			Match(TK_ID);
			Match('(');
			
			// Read in the parameter list
			int params=0;
			while(m_lookahead.type != ')')
			{
				// Read in the expression
				Expr();
				params++;

				if(m_lookahead.type == ')')
					break;
				else
					Match(',');
			}
			Match(')');

			// Get the function's info
			Function * func = &m_S->m_funcs[func_id];

			// Make sure the parameter count is legal
			if((func->limittype == PARAMS_EXACT && params != func->params) ||
			   (func->limittype == PARAMS_MIN && params < func->params) ||
			   (func->limittype == PARAMS_MAX && params > func->params))
			   m_S->Error(line, "'%s()' doesn't take %d parameters", func->name.c_str(), params);

			// Add the opcode to call this function
			AddOpcode(OP_CALL, line, MakeWord(func_id, params));
		}
		// Otherwise, it's a var
		else
		{
			// Push the var's value onto the stack
			AddOpcode(OP_GETVAR, m_lookahead.line, GetVarID());

			// Consume the token
			Match(TK_ID);
		}
	}
	// If the next token's a constant, read it in as such
	// and create a PUSH opcode to push its value onto the stack
	else if(m_lookahead.type == TK_CONST)
	{
		AddOpcode(OP_PUSH, m_lookahead.line, m_lookahead.id);
		Match(TK_CONST);
	}
	// It could also be another whole expression in parentheses.
	else if(m_lookahead.type == '(')
	{
		Match('('); Expr();	Match(')');
	}
	// Otherwise, create an error
	else
		m_S->Error(m_lookahead.line, "Expected identifier, constant or '('");
}



// Returns the variable ID of the next token, assumed to be a variable.
// If it isn't (either by being the wrong token or by not being defined, etc.)
// then it will cause errors.
int Parser::GetVarID()
{
	// Make sure it *is* an identifier
	if(m_lookahead.type != TK_ID)
		// Force an error by Match()ing an ID token
		Match(TK_ID);

	// Get the token's identifier info
	Identifier * id = &m_ids[m_lookahead.id];

	// Get the token's line #
	int line = m_lookahead.line;

	// If it's a variable
	if(id->type == ID_VAR)
		// Return its ID
		return id->id;
	// Else if it's undefined
	else if(id->type == ID_UNDEF)
		m_S->Error(line, "Undeclared identifier '%s'", id->name.c_str());
	// Else it's something else (function perhaps?)
	else
		m_S->Error(line, "Not a variable: '%s'", id->name.c_str());

	// Quiet MSVC's reaction to the lack of other return paths
	return 0;
}



