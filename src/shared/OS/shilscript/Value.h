// Value.h - SHilScript variant value stuff

#ifndef _SHILSCRIPT_VALUE_H_
#define _SHILSCRIPT_VALUE_H_

#include <string>

_BEGIN_SHILSCRIPT

// A variant-type value for use in the VM's stack and for variables.
// It has a simple form of caching so it doesn't have to do costly
// string operations when doing math, or string->number conversions when doing
// string manipulation. Overall it's pretty fast, although I think there
// are things that can be made faster.
class Value
{
private:
	SSString	m_str;		// String value
	Real		m_num;		// Number value
	bool		m_isnum;	// Is numeric?

	bool		m_strok;	// String value up to date?
	bool		m_numok;	// Number value up to date?
	bool		m_isnumok;	// Is the m_isnum flag up-to-date?

private:

	// Basic initialization
	inline void Init() { m_num=0.0f; m_strok = m_numok = m_isnumok = true; }

	// Each of these functions calculates a specific attribute of the value.
	void CalcNumericality();
	void CalcStrFromNum();
	void CalcNumFromStr();

public:
	// Constructors. We can create a value from a string object, a number,
	// or nothing at all (in which case it's all empty.)
	Value() { Init(); }
	Value(SSString val) { Init(); SetStr(val); }
	Value(Real num) { Init(); SetNum(num); }

	// Destructor
	virtual ~Value() {}

	// Calculates all internal info about the value, so that no conversions
	// will have to be made until it is further changed.
	void CalcAllInfo()
	{
		if(!m_numok) CalcNumFromStr();
		if(!m_strok) CalcStrFromNum();
		if(!m_isnumok) CalcNumericality();
	}

	// These functions return various properties of the value (pretty standard stuff here...)
	inline Real GetNum() { if(!m_numok) CalcNumFromStr(); return m_num; }
	inline SSString GetStr() { if(!m_strok) CalcStrFromNum(); return m_str; }
	inline bool IsNumerical() { if(!m_isnumok) CalcNumericality(); return m_isnum; }

	// Sets the value to a numerical value. The string value is invalidated,
	// and the numerical flag is set.
	inline void SetNum(Real num)
	{	
		m_num = num;
		m_isnum = true;
		m_numok = true;
		m_strok = false;
		m_isnumok = true;
	}

	// Sets the value to a string value. Note that this is binary-safe, so you must provide a length.
	inline void SetStr(const char * str, int length)
	{	
		m_str = SSString(str, length);
		m_numok = false;
		m_strok = true;
		m_isnumok = false;
	}

	// Sets the value to a string value, using SSString
	inline void SetStr(SSString str)
	{	
		m_str = str;
		m_numok = false;
		m_strok = true;
		m_isnumok = false;
	}

	// Operator=, for copying the value.
	inline Value & Value::operator=(const Value & other)
	{
		m_isnum = other.m_isnum;
		m_isnumok = other.m_isnumok;
		m_numok = other.m_numok;
		m_strok = other.m_strok;

		if(other.m_strok)
			m_str = other.m_str;

		if(other.m_numok)
			m_num = other.m_num;

		return *this;
	}

	// Checks if two values are equal.
	inline bool Value::IsEqual(Value & other)
	{
		// If they're both numerical, do a numerical test.
		// Otherwise, at least one of them is a string, so compare them string-wise.
		// (Case-sensitive)
		if(IsNumerical() && other.IsNumerical())
			return (GetNum() == other.GetNum());
		else
		{
			// Make sure both string values are set correctly
			if(!m_strok)
				CalcStrFromNum();
			if(!other.m_strok)
				other.CalcStrFromNum();

			// Get both sizes
			int sizea = m_str.size();
			int sizeb = other.m_str.size();

			// First rule out any differences based on size
			if(sizea != sizeb)
				return false;

			// Compare each character
			for(int i=0; i<sizea; i++)
				if(m_str[i] != other.m_str[i])
					return false;

			return true;
		}
	}
};

_END_SHILSCRIPT

#endif



