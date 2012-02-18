#pragma once
#include "HeightGrid.h"
#include "TerrainIndexBlock.h"
#include "VertexBuffer.h"

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

	TerrainPatch* m_parentPatch;

	Vector2D m_texStart;
	Vector2D m_texEnd;

	float drawDistance;
	
public: 

	TerrainMipMapLevel( HeightGrid* aGrid, float spacing);
	TerrainMipMapLevel( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing);
	~TerrainMipMapLevel();

	void Init( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing);
	void Cleanup();

	//float CalcDrawDistance();

	//accessors
	int   Length() ;
	float WorldLength() ;
	float Spacing() ;
	void  Spacing(float value );
	float Interpolation();
	void  Interpolation( float value) ;

	float GetCurrentHeight( int x, int y );
	float GetTargetHeight(int x, int y);
	float GetCurrentInterpolatedHeight( float x, float y );

	TerrainLODVertex* MakeVertices();

	void InitVertexHeights();

	void SetIndexBlockSimplification( int simplifyWest, int simplifyEast, int simplifyNorth, int simplifySouth );
	void UpdateIndices();
	void UpdateVertexHeights();

	void Draw();

};