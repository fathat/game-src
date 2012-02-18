#pragma once
#include "HeightGrid.h"
#include "TerrainIndexBlock.h"

class TerrainPatch;

class TerrainMipMapLevel
{
	//The grid that makes up this terrain block
	HeightGrid* m_baseGrid;
	HeightGrid* m_simplifiedGrid;

	float m_spacing;
	float m_interpolation;

	TerrainLODVertex* m_vertices;
	TerrainIndexBlock* m_indexBlock;

	VertexBuffer m_vertexBuffer;
	VertexBuffer m_targetVertexBuffer;

	Screen3D* m_screenPtr;

	TerrainPatch* m_parentPatch;

	D3DXVECTOR2 m_texStart;
	D3DXVECTOR2 m_texEnd;

	float drawDistance;
	
public: 

	TerrainMipMapLevel( HeightGrid* aGrid, float spacing, Screen3D* screen);
	TerrainMipMapLevel( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing, Screen3D* screen);
	~TerrainMipMapLevel();

	void Init( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing, Screen3D* screen);
	void Cleanup();

	float CalcDrawDistance();

	//accessors
	int   Length() ;
	float WorldLength() ;
	float Spacing() ;
	void  Spacing(float value );
	float Interpolation();
	void  Interpolation( float value) ;

	float GetCurrentInterpolatedHeight( float x, float y );

	TerrainLODVertex* MakeVertices();

	void InitVertexHeights();

	void SetIndexBlockSimplification( int simplifyWest, int simplifyEast, int simplifyNorth, int simplifySouth );
	void UpdateIndices();
	void UpdateVertexHeights();

	void Draw();

};