#include "TerrainMap.h"
#include "TerrainPatch.h"
#include "..\CVAR\Cvar.h"

TerrainMap::TerrainMap( Screen3D* screen,  HeightGrid* aHeightGrid, int patchSize )
{
    m_screen = screen;
	m_baseHeightGrid = aHeightGrid->Duplicate();
	m_spacing = 1.0f;

	//compile shader
	CVar.WriteLine	( "Compiling terrain shader" );
	CompileShader();

	//init patches array
	if( patchSize == 0 )
		patchSize = 17;
	m_patchesAcross = (m_baseHeightGrid->Size()-1)/(patchSize-1);;
	m_patches	 = new TerrainPatch*[ m_patchesAcross*m_patchesAcross ];

	//get the height grids
	CVar.WriteLine( "Creating terrain patches" );
	HeightGrid** hgs = aHeightGrid->Fracture( m_patchesAcross );

	//CVar.WriteLine( "..." );
	for( int i=0; i< m_patchesAcross*m_patchesAcross; i++)
	{
		m_patches[i] = new TerrainPatch( this, screen, hgs[i]);
		//CVar.WriteLine("+");
		delete hgs[i];
	}
	delete[] hgs;

	

	ArrangePatches();
}

TerrainMap::~TerrainMap()
{
	for(int i=0; i<m_patchesAcross*m_patchesAcross; i++)
		delete m_patches[i];
	//delete []m_patches;

}

void TerrainMap::Spacing( float spacing ) 
{ 
	m_spacing = spacing; 
	for(int i=0; i<m_patchesAcross*m_patchesAcross; i++)
		m_patches[i]->SpacingBetweenVertices( spacing );

	ArrangePatches();
}

void TerrainMap::CompileShader()
{
	//Shader declaration
	D3DVERTEXELEMENT9 shaderDeclaration[] = 
	{
		//stream 0
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		//stream 1
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
		{ 1, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		{ 1, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5 },
		D3DDECL_END()
	};

	m_vertexShader.Init( m_screen, "..\\scripts\\shaders\\landscape.nvv", shaderDeclaration );

	

}

void TerrainMap::ArrangePatches()
{
	//arrange patches
	for(int y=0; y< m_patchesAcross; y++)
	{
		for(int x=0; x< m_patchesAcross; x++)
		{
			TerrainPatch* patch = m_patches[x+y*m_patchesAcross];
			patch->Column (x);
			patch->Row    (y);
			patch->Offset (Vector3D( x*patch->WorldLength(), 0, y*patch->WorldLength() ) );
			patch->Refresh();
		}
	}
}

void TerrainMap::UpdateLOD(Vector3D* camera)
{
	for(int y=0; y< m_patchesAcross; y++)
	{
		for(int x=0; x< m_patchesAcross; x++)
		{
			TerrainPatch* patch = m_patches[x+y*m_patchesAcross];
			
			//figure out distance to patch
			float distance = (patch->Center() - (*camera)).Mag();

			patch->LOD( (distance-1000) / 1000.0f );
			//patch->LOD( 0.0f );
		}
	}

}

TerrainPatch* TerrainMap::GetWestNeighborOf( int x, int y ) {
	if( x > 0 ) return m_patches[(x-1)+y*m_patchesAcross];
	return NULL;
}
TerrainPatch* TerrainMap::GetEastNeighborOf( int x, int y ) {
	if( x < (m_patchesAcross-1) ) return m_patches[(x+1)+y*m_patchesAcross];
	return NULL;
}

TerrainPatch* TerrainMap::GetNorthNeighborOf( int x, int y ) {
	if( y > 0 ) return m_patches[x+(y-1)*m_patchesAcross];
	return NULL;
}
TerrainPatch* TerrainMap::GetSouthNeighborOf( int x, int y ) {
	if( y < m_patchesAcross-1 ) return m_patches[x+(y+1)*m_patchesAcross];
	return NULL;
}

void TerrainMap::ClearFlags() 
{
	for( int i=0; i< this->m_patchesAcross* m_patchesAcross; i++ )
		m_patches[i]->ClearFlags();
}

void TerrainMap::Update()
{
	Reindex();
	//for( int i=0; i< this->m_patchesAcross* m_patchesAcross; i++ )
	//	m_patches[i]->Refresh();

	ClearFlags();
}

void TerrainMap::ResetLOD()
{
	for( int i=0; i< this->m_patchesAcross* m_patchesAcross; i++ )
		m_patches[i]->LOD( 0 );

}
TerrainPatch* TerrainMap::GetPatchAt( int col, int row )
{
	return m_patches[col+row*m_patchesAcross ];
}
void TerrainMap::SetLODOnPatchAt( int x, int y, float LOD )
{
	m_patches[x+y*m_patchesAcross ]->LOD( LOD );
}

void TerrainMap::Reindex()
{
	for( int i=0; i< m_patchesAcross* m_patchesAcross; i++ )
		m_patches[i]->CheckForLODConsistency();
	
}

void TerrainMap::DrawAll()
{
	//Set the vertex shader
	//m_screen->SetFVF( D3DFVF_TERRAINLODVERTEX);
	//m_screen->D3DDevice->SetVertexShader( this->m_vertexShader);
	//m_screen->D3DDevice->SetVertexDeclaration( this->ShaderDeclaration() );
	m_vertexShader.Activate();

	for( int i=0; i< m_patchesAcross* m_patchesAcross; i++ )
		m_patches[i]->Draw();
	//m_patches[1]->Draw();
	//m_patches[2]->Draw();

	m_screen->D3DDevice->SetVertexDeclaration( NULL );
	
}