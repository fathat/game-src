// Vector.h - Data vector stuffage

#ifndef _SHILSCRIPT_VECTOR_H_
#define _SHILSCRIPT_VECTOR_H_

_BEGIN_SHILSCRIPT

// Generic vector class, a lot like std::vector. Also, unlike std::vector,
// it has a granularity option, so you can set it to only resize at certain
// intervals. Perhaps in the future it will resize exponentially, like 16->32->64->etc. ?
template<class Type, int Granularity> class Vector
{
protected:
	Type *				m_Data;	// Actual data array
	unsigned int		m_Size;	// Length of the data (valid part)
	unsigned int		m_Cap;	// Capacity of the data buffer (entire thing)

public:

	// Constructor
	inline Vector()
	{
		m_Data = NULL;
		m_Size = 0;
		m_Cap = 0;
	}

	// Destructor
	inline virtual ~Vector()
	{
		Clear();
	}

	// Simple querying stuff
	inline unsigned int Size() const { return m_Size; }
	inline bool Empty() const { return !m_Size; }
	inline unsigned int Capacity() const { return m_Cap; }

	// Operator so you can treat it like a normal C array (i.e. array[i])
	inline Type & operator[](unsigned int id)
	{
		return m_Data[id];
	}

	// Adds an item to the end of the vector.
	// It returns the index of the item added.
	inline unsigned int PushBack(const Type & NewItem)
	{
		// If we don't have enough room, increase our room
		if(m_Size+1 > m_Cap)
			Reserve(m_Cap + Granularity);

		// Copy this new data over
		m_Data[m_Size++] = NewItem;

		// Return the ID of this item
		return m_Size-1;
	}

	// Removes an item from the end of the vector.
	// Currently it doesn't size the vector down, as we probably
	// will return to at least this level again anyways
	inline void PopBack()
	{
		// Decrease the apparent length, that's all for now :)
		if(m_Size)
			m_Size--;
	}

	// Clears out the array. 
	inline void Clear()
	{
		// Delete the data
		if(m_Data) delete [] m_Data;

		// Reset all members
		m_Data = NULL;
		m_Size = 0;
		m_Cap = 0;
	}

	// Copies the array
	inline Vector<Type,Granularity>& operator=(const Vector<Type,Granularity>& other)
	{
		// Clear out this array
		Clear();

		// Copy the other data over
		m_Size = other.m_Size;
		m_Cap = other.m_Cap;
		if(other.m_Data)
			m_Data = new Type[m_Cap];
		else
			m_Data = 0;

		// Copy the actual data array over
		for(unsigned int i=0; i<m_Size; i++)
			m_Data[i] = other.m_Data[i];

		// Return a reference to this
		return *this;
	}

	// Increases the capacity of the data buffer
	inline void Reserve(unsigned int NewCap)
	{
		// If we're trying to make it smaller, fail
		if(m_Cap >= NewCap)
			return;

		// Allocate new memory
		Type * NewData = new Type[NewCap];

		// Copy all the old stuff over.
		// TODO: Speed up with direct constructor calling, perhaps?
		for(unsigned int i=0; i<m_Size; i++)
			NewData[i] = m_Data[i];

		// Save the new capacity
		m_Cap = NewCap;

		// Delete the old data buffer
		if(m_Data)	delete [] m_Data;

		// Replace it with the new one
		m_Data = NewData;
	}
};

_END_SHILSCRIPT

#endif
