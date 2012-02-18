// I N C L U D E S /////////////////////////////////////
#pragma once
#include "vertexbuffer.h"


// D E F I N E S ///////////////////////////////////////


// C L A S S E S ///////////////////////////////////////

class CombinationBuffer
{
public:
	VertexBuffer FinalVB;
	IndexBuffer  FinalIB;
	int CurrentVBIndex;
	int CurrentIBIndex;

public:
	bool Init( Screen3D& Screen, int MaxSize, int MaxIBSize );
	void Shutdown( );

	bool AddVertices( CustomVertex* list, int nVertices, WORD* indices, int nIndices );
	void DrawBuffer(Screen3D& Screen);
	void FlushBuffer();
};