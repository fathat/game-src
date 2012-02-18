// image.h
// Multi-format image loader thingy
// (C) 2001 Scott Hilbert

#pragma once

#include <wtypes.h>

struct ARGB_PIXEL
{
	BYTE b,g,r,a;
};

struct CGenImage	// Generic Image
{
// Member vars
private:
	ARGB_PIXEL *	m_pBits;		// Actual image data
	UINT			m_Width;		// Image width
	UINT			m_Height;		// Image height
	BOOL			m_Alpha;		// Alpha data?

// Member functions
public:
	// Lifetime
	CGenImage();
	~CGenImage();

	// Loading
	HRESULT Load(LPCSTR Filename);

	// Data access
	BOOL HasAlphaData();
	UINT GetWidth();
	UINT GetHeight();
	ARGB_PIXEL * GetBits();
};
