// LLStack.h - Linked list stack, as opposed to granular stack from Stack.h

#ifndef _SHILSCRIPT_LLSTACK_H_
#define _SHILSCRIPT_LLSTACK_H_

_BEGIN_SHILSCRIPT

// Linked-List based stack. This is different from the normal Stack because
// it specifically specifies to be done in a linked-list fashion. This is necessary
// (or recommended) for some types that would otherwise allocate a crapload of memory
// if granulary.
template<class Type> class LLStack
{
private:
	// Item structure used as a node in thethe stack list
	struct Item
	{
		Type	data;	// Data this item node carries
		Item*	next;	// Pointer to the next item
	};

	Item* m_top;			// Top stack item. NULL means there is nothing in the stack.
	unsigned int m_size;	// Size of the stack

public:

	// Constructor
	inline LLStack() { m_top = 0; m_size = 0; }

	// Destructor
	virtual inline ~LLStack() { Clear(); }

	// Pushes a copy of the specified object onto the stack
	inline void Push(const Type& item)
	{
		// Create a new item structure, linking it to the current top one
		Item * newitem = new Item;
		newitem->data = item;
		newitem->next = m_top;

		// Set it as the top
		m_top = newitem;

		// Increase the size counter
		m_size++;
	}

	// Pushes a new item onto the stack, leaving the associated
	// data in its default state
	inline void Push()
	{
		// Create a new item (leave the data member how it is)
		Item * newitem = new Item;
		newitem->next = m_top;

		// Set it as the top item
		m_top = newitem;

		// Increase the size counter
		m_size++;
	}

	// Pops the top item off the stack
	inline void Pop()
	{
		// Save the old top item
		Item * oldtop = m_top;

		// Set the new top as the old top's NEXT item
		m_top = oldtop->next;

		// Delete the old top item
		delete oldtop;

		// Decrease the size
		m_size--;
	}

	// Returns a reference to the top item on the stack
	inline Type& Top()
	{
		return m_top->data;
	}

	// Deletes the entire stack
	inline void Clear()
	{
		Item * curr = m_top,	// Current item
			 * next;			// Next item

		// While there are items left to delete
		while(curr)
		{
			// Save the ptr to the next item
			next = curr->next;

			// Delete the current item
			delete curr;
			
			// Move to the next item
			curr = next;
		}

		// Set the size to zero
		m_size = 0;
	}

	// Returns whether or not the stack is empty
	inline bool Empty() { return !m_top; }

	// Returns the number of items in the stack
	inline unsigned int Size() { return m_size; }

	// Copies the stack
	inline LLStack<Type>& operator=(const LLStack<Type>&other)
	{
		// Clear out this stack
		Clear();

		Item	* curr = other.m_top,	// Current item from the source (other) stack
				* prev=0,				// Previous item in the destination stack
				* newitem;				// Ptr to the new item we're working with

		// For each item in the source stack
		while(curr)
		{
			// Create a new item, copying the item from the source =
			newitem = new Item;
			newitem->data = curr->data;
			newitem->next = 0;

			// If there's a previous item, set its next item pointer to the new item
			if(prev)
				prev->next = newitem;
			// Otherwise, there was no previous item, so this item must be the top
			else
				m_top = newitem;

			// The newest item is now the previous item
			prev = newitem;

			// Move to the next source item
			curr = curr->next;
		}

		// Copy the size from the other stack (they should be the same)
		m_size = other.m_size;

		// Return a reference to this object
		return *this;
	}
};

_END_SHILSCRIPT

#endif

