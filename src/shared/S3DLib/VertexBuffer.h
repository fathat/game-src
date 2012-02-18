// I N C L U D E S //////////////////////////////
#pragma once
#include <assert.h>
#include "screen.h"
#include "types.h"

// D E F I N E S ////////////////////////////////
//typedef unsigned int Index32;
//typedef WORD Index16;

#define Index16 WORD
#define Index32 DWORD

enum IndexBufferBitWidth { IB16Bit, IB32Bit };

// C L A S S E S ////////////////////////////////


//////// IndexBuffer //////////////////////////
//
// Description:
//
// Buffer used to hold vertex indices into
// a vertex buffer. This is sent to D3D for
// use with the DrawIndexedPrimitive call.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - !
//
///////////////////////////////////////////////


class IndexBuffer
{
protected:

	//D3D8 index buffer that this encapsulates
	LPDIRECT3DINDEXBUFFER9 lpIndexBuffer;
	IndexBufferBitWidth bitwidth;

	Screen3D* ScreenPtr;

	//just the number of indices
	int nIndices;

public:

	//constructor/destructor
	IndexBuffer() {lpIndexBuffer = NULL; nIndices = NULL; }
	~IndexBuffer() {Destroy();}

	//init/destroy functions
	bool Init(Screen3D& Screen, int size, IndexBufferBitWidth bw);
	void Destroy();


	void operator = (  IndexBuffer& IB ) ;

	int GetIndexCount() { return nIndices; }
	void Lock(Index16** Buffer);
	void Lock(Index32** Buffer);
	void Unlock();

	IDirect3DIndexBuffer9* GetD3DBufferInterface() { return lpIndexBuffer; }

};




//////// VertexBuffer /////////////////////////
//
// Description:
//
// Buffer used to hold vertices. This can be
// drawn straight out on it's own, but generally
// you should use an IndexBuffer with this (so
// that you can avoid duplicate vertex sets).
//
// Known Problems:
//
// - None
//
// TODO:
//
// - !
//
///////////////////////////////////////////////


class VertexBuffer
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_VB;
	int m_nVertices;
	
	Screen3D* m_screenPtr;
	D3DPOOL   m_pool;
	DWORD     m_fvf;

	int m_vertexSize;
		
public:

	//constructor/destructor
	VertexBuffer();
	~VertexBuffer();


	//init the vertex buffer. You can give it a VertexList to
	//initialize with (optionally).

	bool InitDynamic( Screen3D& screen, DWORD FVF, int vertexSize, int vertexCount ) ;
	bool Init (Screen3D& Screen, DWORD FvF, int vSize, int nVertex, bool ParticleBuffer=false, bool Dynamic=true, _D3DPRIMITIVETYPE Type= D3DPT_TRIANGLELIST, bool ForceSoftware=false);
	void Cleanup ();

	//Use this to get a pointer to the data
	bool Lock(BYTE** Vertex, DWORD Flags = NULL);
	void Unlock();

	void FillBuffer( BYTE* VertexList, int nVert );

	void SetActive( int strm=0 ) { m_screenPtr->D3DDevice->SetStreamSource( 0, m_VB, 0, m_vertexSize ); }

	IDirect3DVertexBuffer9* D3DBuffer() { return m_VB; }
	int  VertexCount() { return m_nVertices; }
	int  VertexSize() { return m_vertexSize; } 
	DWORD FVF() { return m_fvf; }
	
	//transformations
	bool Process( Screen3D& Screen, VertexBuffer& DestBuffer );

	void Optimize( Screen3D& Screen );
	
	//draws the vertex buffer just as a bunch of triangles.
	void Draw(Screen3D& Screen);
	
	void  operator = (  VertexBuffer& v ) ;

};

