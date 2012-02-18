#pragma once
#include "HeightGrid.h"


//forward declarations
class TerrainPatch;

/// <summary>
/// Summary description for TerrainMap.
/// </summary>
class TerrainMap
{
	int m_patchesAcross;

	TerrainPatch** m_patches;

	HeightGrid* m_baseHeightGrid;

	float m_spacing;

	void CompileShader();

public:
	

	TerrainMap(  HeightGrid* aHeightGrid, int patchSize );
	~TerrainMap();
	
	void UpdateLOD( Vector3D* camera ) ;

	void ArrangePatches();

	int PatchesAcross() { return m_patchesAcross; }

	TerrainPatch* GetWestNeighborOf( int x, int y );
	TerrainPatch* GetEastNeighborOf( int x, int y ) ;
	TerrainPatch* GetNorthNeighborOf( int x, int y ) ;
	TerrainPatch* GetSouthNeighborOf( int x, int y );

	void ClearFlags() ;
	void Update();

	void ResetLOD();
	TerrainPatch* GetPatchAt( int col, int row );
	void SetLODOnPatchAt( int x, int y, float LOD );

	void Spacing( float spacing );
	float Spacing() { return m_spacing; }

	void Reindex();
	
	void DrawAll();
};