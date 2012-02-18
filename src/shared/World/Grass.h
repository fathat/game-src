#pragma once
#include "..\TerrainLOD\HeightGrid.h"
#include "..\S3DLib\VertexBuffer.h"
#include "..\S3DLib\Screen.h"
#include "..\S3DLib\VertexShader.h"

class QuadNode;

class GrassPatch
{
	VertexBuffer m_vertexBuffer;
	IndexBuffer  m_indexBuffer;
	HeightGrid*  m_opacityMap;

	Screen3D* m_screen;

	TextureHandle m_grassTexture;
	int nPolygons;

public:

	GrassPatch();
	~GrassPatch();

	bool Init( Screen3D* screen, QuadNode* aNode, int density );
	
    void Draw();

};