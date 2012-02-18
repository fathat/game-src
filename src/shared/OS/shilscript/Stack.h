// Stack.h - Stack type

#ifndef _SHILSCRIPT_STACK_H_
#define _SHILSCRIPT_STACK_H_

#include "Vector.h"

_BEGIN_SHILSCRIPT

// Stack Granularity
const int SG = 25;

// Generic stack class. This is really a vector, basically because:
// (1) it's faster not to reallocate memory all the time, and the granular vector class really helps here
// (2) I didn't feel like rewriting the granular functionality :)
template<class Type> class Stack : private Vector<Type, SG>
{
public:
	// Constructor
	Stack() {}

	// Destructor
	virtual ~Stack() {}

	// Pushes an item onto the stack
	inline void Push(const Type & Item) { PushBack(Item); }

	// Pops an item off of the stack
	inline void Pop() { PopBack(); }

	// Returns a reference to the top item on the stack
	inline Type & Top() { return m_Data[m_Size-1]; }

	// Copies a stack
	inline Stack<Type>& operator=(const Stack<Type>& other) { Vector<Type, SG>::operator =(other); return *this; }

	// Returns a boolean value representing if the stack is empty
	inline bool Empty() { return Vector<Type, SG>::Empty(); }

	// Returns the size of the stack
	inline unsigned int Size() { return Vector<Type, SG>::Size(); }

	// Clears out the stack
	inline void Clear() { Vector<Type, SG>::Clear(); }
};

_END_SHILSCRIPT

#endif

