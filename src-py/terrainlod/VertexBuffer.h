#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "MathTypes.h"

struct TerrainLODVertex
{
	Vector3D v;
	Vector2D tex0;
	Vector2D tex1;
	Vector2D tex2;

	TerrainLODVertex()
	{
	}

	TerrainLODVertex( const Vector3D& _v)
	{
		v = _v;
	}
};

class VertexBuffer
{
public:
	TerrainLODVertex* vertexArray;
	int nVertices;

public:
	VertexBuffer();
	VertexBuffer(int nvertices);
	~VertexBuffer();

	bool Init(int nvertices);
};

#endif
