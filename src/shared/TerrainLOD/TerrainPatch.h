#pragma once
#include <vector>
using namespace std;
#include "HeightGrid.h"
#include "TerrainMipMapLevel.h"
#include "LODDescription.h"

class TerrainMap;

class TerrainPatch
{
	//The height map all this is based on. 
	HeightGrid* m_baseHeightGrid;

	Screen3D* m_screen;

	vector<TerrainMipMapLevel*> m_mipLevels;


	LODDescription* m_LODDesc;

	//neighbors. TerrainMap instance should handle linking these.
	TerrainMap* m_parentTerrain;
	

	//the lowest LOD is the point at which we have a 1 triangle X 1 triangle
	//patch.
	int m_lowestLOD;//=0;

	//Current level of detail. Note that it's a float, since it can hover
	//between two LODs (trilinear filtering! yay!)
	float m_LOD;//=0;

	int m_row,//=-1,
		m_column;//=-1;
	

	bool m_indicesChanged;//=false;

	float m_spacing;//=1.0f;

	Vector3D m_offset;


public:
	void ClearFlags();

	Vector3D Center();

	//Accessors
	TerrainMap* ParentTerrain() ;
	void ParentTerrain(TerrainMap* value) ;

	int Row() ;
	void Row( int value ) ;

	int Column() ;
	void Column( int value );

	float LOD() ;
	void LOD(float value );
	
	float SpacingBetweenVertices();
	void SpacingBetweenVertices( float value);

	HeightGrid* Heights() { return m_baseHeightGrid; }

	
	Vector3D Offset();
	void Offset( Vector3D& value) ;

	float WorldLength() ;

	void Rescale();

	TerrainPatch( Screen3D* screen,  HeightGrid* hg );
	TerrainPatch (TerrainMap* t, Screen3D* screen,  HeightGrid* hg );
	~TerrainPatch();

	void Init ( Screen3D* screen, HeightGrid* hg );
	void Cleanup();

	void SetIndexSimplification();

	void SynchronizeDescWithState();

	void CheckForLODConsistency();

	void RecalcIndexBuffer();

	void Refresh();

	void Draw();
	void DrawLevel( float LOD );
};