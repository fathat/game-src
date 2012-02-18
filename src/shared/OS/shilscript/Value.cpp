// Value.cpp - SHilScript values

#include "Globals.h"
#include "Value.h"
#include <ctype.h>

_USING_SHILSCRIPT

// TODO: Recognize [whitespace] [digits] [.] [digits] [d D e E] [sign] [digits]
// TODO: This needs to be done ASAP. Or we need an altogether different way of
// TODO: handling number <-> text conversion. This really cripples SHilScript
// TODO: as far as high numbers.
void Value::CalcNumericality()
{
	if(!m_strok)
		CalcStrFromNum();

	m_isnumok = true;

	// If the string is empty, return that this is numerical
	// (atof() will return zero anyway)
	if(!m_str.size())
	{
		m_isnum = true;
		return;
	}

	// Check if it's all digits plus at most one decimal
	int decimal=0;

	// For each character
	for(int i=0; i<(signed)m_str.size(); i++)
	{
		// If it's a decimal
		if(m_str[i] == '.')
		{
			// Increase the decimal count
			decimal++;

			// If the # of decimals is greater than 1
			if(decimal > 1)
			{
				// This can't be a number
				m_isnum = false;
				return;
			}
		}
		// Otherwise, if this character's not a digit
		else if(!isdigit(m_str[i]))
		{
			// This can't be a number
			m_isnum = false;
			return;
		}
	}

	// It has to be a number!
	m_isnum = true;
}

void Value::CalcNumFromStr()
{
	// If it's numerical, convert it to a floating-point
	// value; otherwise return one. Note that IsNumerical()
	// returns true if the string is empty and therefore
	// the null string "" is equivalent to zero.

	if(IsNumerical()) m_num = (Real)atof(m_str.c_str());
	else m_num = 1.0f;

	m_numok = true;
}

void Value::CalcStrFromNum()
{
	// Convert from float to string
	char buf[64];
	sprintf(buf, "%g", (double)m_num);

	// Save this string in the buffer
	m_str = SSString(buf);

	m_strok = true;
}




