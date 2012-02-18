#pragma once
#include "..\S3DLib\Screen.h"
#include "..\S3DLib\VertexShader.h"
#include "HeightGrid.h"


//forward declarations
class TerrainPatch;

/// <summary>
/// Summary description for TerrainMap.
/// </summary>
class TerrainMap
{
	int m_patchesAcross;
	Screen3D* m_screen;

	TerrainPatch** m_patches;

	HeightGrid* m_baseHeightGrid;

	VertexShader m_vertexShader;

	//IDirect3DVertexShader9* m_vertexShader;
	//LPDIRECT3DVERTEXDECLARATION9 m_shaderDeclaration;

	float m_spacing;

	void CompileShader();

public:
	

	TerrainMap( Screen3D* screen,  HeightGrid* aHeightGrid, int patchSize );
	~TerrainMap();

	LPDIRECT3DVERTEXDECLARATION9 ShaderDeclaration() { return m_vertexShader.VertexDeclaration(); }
	
	void UpdateLOD( Vector3D* camera ) ;

	void ArrangePatches();

	int PatchesAcross() { return m_patchesAcross; }

	TerrainPatch* GetWestNeighborOf( int x, int y );
	TerrainPatch* GetEastNeighborOf( int x, int y ) ;
	TerrainPatch* GetNorthNeighborOf( int x, int y ) ;
	TerrainPatch* GetSouthNeighborOf( int x, int y );

	void ClearFlags() ;
	void Update();

	IDirect3DVertexShader9* Shader() { return m_vertexShader.D3DShader(); }

	void ResetLOD();
	TerrainPatch* GetPatchAt( int col, int row );
	void SetLODOnPatchAt( int x, int y, float LOD );

	void Spacing( float spacing );
	float Spacing() { return m_spacing; }

	void Reindex();
	
	void DrawAll();
};