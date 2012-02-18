// Parser.h - SHilScript syntax parsing phase

#ifndef _SHILSCRIPT_PARSER_H_
#define _SHILSCRIPT_PARSER_H_

#include "Lexer.h"
#include "Opcodes.h"
#include "Identifier.h"
#include "Stack.h"

_BEGIN_SHILSCRIPT

// prototype
class State;

// Takes care of parsing
class Parser
{
private:
	int						m_scope;		// Scope level
	State*					m_S;			// Parent SHilScript state
	Lexer					m_L;			// Lexer
	Vector<Identifier,50>	m_ids;			// Identifier name lookup
	Stack<int>				m_breakstack;	// Stack of break labels
	Stack<int>				m_contstack;	// Stack of continue labels
	bool					m_infunc;		// In a function?
	int						m_funcvars;		// Number of variables declared in this function

	Token	m_lookahead;		// First lookahead token
	Token	m_lookahead2;		// 2nd lookahead token
public:
	Parser(State * S, Inputter * inputter);
	virtual ~Parser();

	void NextTok();
	void Match(int tk);
	int AddOpcode(int id, int line, int val);
	int	AddID(const char * buf);
	void ScopeUp();
	void ScopeDown();

	void Start();
	
	void Stmt();	
	void IfStmt();
	void WhileStmt();
	void DoStmt();
	void ForStmt();
	void ContinueStmt();
	void BreakStmt();
	void DeclStmt();
	void FuncStmt();
	void ReturnStmt();
	void CompoundStmt();
	void PrototypeStmt();
	void ExitStmt();
	void ExprStmt();

	void Expr();
	void AssignExpr();
	void BinaryExpr(int row);
	void UnaryExpr();
	void BasicExpr();
	int GetVarID();
};

_END_SHILSCRIPT

#endif


