#include "TerrainMap.h"
#include "TerrainPatch.h"

TerrainMap::TerrainMap( HeightGrid* aHeightGrid, int patchSize )
{
    
	m_baseHeightGrid = aHeightGrid->Duplicate();
	m_spacing = 1.0f;

	//compile shader
	CompileShader();

	//init patches array
	if( patchSize == 0 )
		patchSize = 17;
	m_patchesAcross = (m_baseHeightGrid->Size()-1)/(patchSize-1);;
	m_patches	 = new TerrainPatch*[ m_patchesAcross*m_patchesAcross ];

	//get the height grids
	HeightGrid** hgs = aHeightGrid->Fracture( m_patchesAcross );

	for( int i=0; i< m_patchesAcross*m_patchesAcross; i++)
	{
		m_patches[i] = new TerrainPatch( this, hgs[i]);
		delete hgs[i];
	}
	delete[] hgs;

	

	ArrangePatches();
}

TerrainMap::~TerrainMap()
{
	for(int i=0; i<m_patchesAcross*m_patchesAcross; i++)
		delete m_patches[i];
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

			//patch->LOD( distance / 1000.0f );
			patch->LOD( 0.0f );
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
	for( int i=0; i< m_patchesAcross* m_patchesAcross; i++ )
		m_patches[i]->Draw();
	
}