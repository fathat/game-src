#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() 
{
	nVertices = 0;
	vertexArray = 0;
}

VertexBuffer::VertexBuffer(int nvertices)
{
	Init(nvertices);
}

bool VertexBuffer::Init(int nvertices) {
	nVertices = nvertices;
	this->vertexArray = new TerrainLODVertex[nVertices];
	return true;
}

VertexBuffer::~VertexBuffer() {
}