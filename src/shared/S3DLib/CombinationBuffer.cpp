#include "CombinationBuffer.h"

bool CombinationBuffer::Init( Screen3D& Screen, int MaxSize, int MaxIBSize )
{
	FinalVB.Init( Screen, D3DFVF_CUSTOMVERTEX, sizeof(CustomVertex), MaxSize);
	FinalIB.Init( Screen, MaxIBSize );

	return true;
}

void CombinationBuffer::Shutdown( )
{
}

bool CombinationBuffer::AddVertices( CustomVertex* list, int nVertices, WORD* indices, int nIndices )
{
	CustomVertex* destlist;
	WORD* destiblist;
	FinalVB.Lock((BYTE**)&destlist);
	FinalIB.Lock(&destiblist);

	
	memcpy(&destlist[CurrentVBIndex], list, sizeof(CustomVertex) * (nVertices-CurrentVBIndex));
	memcpy(&destiblist[CurrentIBIndex], indices, sizeof(WORD) * (nIndices-CurrentIBIndex));

	FinalIB.Unlock();
	FinalVB.Unlock();

	return true;
}

void CombinationBuffer::DrawBuffer(Screen3D& Screen )
{
	Screen.DrawPrimitiveVB( &FinalVB, &FinalIB, D3DPT_TRIANGLELIST, D3DFVF_CUSTOMVERTEX, 0, CurrentIBIndex); 
}

void CombinationBuffer::FlushBuffer()
{
	CurrentIBIndex=0;
	CurrentVBIndex=0;
}