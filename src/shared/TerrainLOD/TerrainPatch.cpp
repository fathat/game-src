#include "TerrainMap.h"
#include "TerrainPatch.h"
#include "..\CVAR\CVar.h"

void TerrainPatch::ClearFlags()
{
	m_indicesChanged = false;
}

Vector3D TerrainPatch::Center()
{
	Vector3D center ( WorldLength() /2, 0, WorldLength()/2 );
	return (m_offset + center);
}

//Accessors
TerrainMap* TerrainPatch::ParentTerrain() { 
	return m_parentTerrain;
}

void TerrainPatch::ParentTerrain(TerrainMap* value) { 
	m_parentTerrain = value ; 
}

int TerrainPatch::Row() {
	return m_row;  
}

void TerrainPatch::Row( int value ) {
	m_row = value; 
}

int TerrainPatch::Column() {
	return m_column;  
}

void TerrainPatch::Column( int value )
{
	m_column = value;
}

float TerrainPatch::LOD() {
	return m_LOD; 
}

void TerrainPatch::LOD(float value )
{
	m_LOD = value; 
	if( m_LOD < 0.0f)  m_LOD = 0.0f; 
	if( m_LOD >= m_lowestLOD-1 ) m_LOD = (float)m_lowestLOD-1;
}

float TerrainPatch::SpacingBetweenVertices() {
	return m_spacing; 
}

void TerrainPatch::SpacingBetweenVertices( float value)
{
	m_spacing = value; 
	Rescale();
}


Vector3D TerrainPatch::Offset() {
	return m_offset; 
}

void TerrainPatch::Offset( Vector3D& value) 
{
	m_offset = value; 
}

float TerrainPatch::WorldLength() {
	return m_mipLevels[0]->WorldLength(); 
}

void TerrainPatch::Rescale()
{
	for( unsigned int i=0; i<m_mipLevels.size(); i++ )
		m_mipLevels[i]->Spacing (  (float)pow(2.0f, (float)i ) * m_spacing );
}

TerrainPatch::TerrainPatch( Screen3D* screen,  HeightGrid* hg )
{
	Init( screen, hg );
}

TerrainPatch::TerrainPatch (TerrainMap* t, Screen3D* screen,  HeightGrid* hg )
{
	m_parentTerrain = t;
	Init( screen, hg );
}

TerrainPatch::~TerrainPatch()
{
	Cleanup();
}


void TerrainPatch::Init ( Screen3D* screen, HeightGrid* hg )
{
	//set some defaults
	m_lowestLOD	=0;
	m_LOD		=0;
	m_row		=-1;
	m_column	=-1;
	m_indicesChanged=false;
	m_spacing	=1.0f;
	m_LODDesc	= NULL;

	//CVar.WriteLine("Making patch..." );

	m_screen = screen;
	m_baseHeightGrid = hg->Duplicate();
	
	m_lowestLOD = (int)(log((float)hg->SquaresAcross())/ log( (float)2 )) -1 ;//+ 1;

	HeightGrid* grid = m_baseHeightGrid;

	//CVar.WriteLine( "%d miplevels", m_lowestLOD );
	
	for( int i=0; i<m_lowestLOD; i++ )
	{				
		//CVar.WriteLine( "%d", i );
		m_mipLevels.push_back( new TerrainMipMapLevel( this, grid, (float)pow(2.0, i ), screen ) );
		//MEMORY LEAK
		grid = grid->HalfLOD();
	}


	//CVar.WriteLine( "Made miplevels" );

}

void TerrainPatch::Cleanup()
{
	delete m_baseHeightGrid;

	for( unsigned int i=0; i<m_mipLevels.size(); i++)
	{
		TerrainMipMapLevel* pMip = m_mipLevels[i];
		delete pMip;
	}
	m_mipLevels.clear();

}



void TerrainPatch::SetIndexSimplification()
{
	TerrainPatch* west  = m_parentTerrain->GetWestNeighborOf ( m_column, m_row );
	TerrainPatch* east  = m_parentTerrain->GetEastNeighborOf ( m_column, m_row );
	TerrainPatch* north = m_parentTerrain->GetNorthNeighborOf ( m_column, m_row );
	TerrainPatch* south = m_parentTerrain->GetSouthNeighborOf ( m_column, m_row );


	m_mipLevels[ (int) m_LOD]->SetIndexBlockSimplification( 
		(west  != NULL) ? (int)( (int)west->LOD()  - (int)m_LOD) : 0,
		(east  != NULL) ? (int)( (int)east->LOD()  - (int)m_LOD) : 0,
		(north != NULL) ? (int)( (int)north->LOD() - (int)m_LOD) : 0,
		(south != NULL) ? (int)( (int)south->LOD() - (int)m_LOD) : 0);

}

void TerrainPatch::SynchronizeDescWithState()
{
	m_LODDesc->LoadValuesFromPatch( this );
}

void TerrainPatch::CheckForLODConsistency()
{
	if (m_LODDesc == NULL ) 
	{
		m_LODDesc = new LODDescription( this );
		RecalcIndexBuffer();
	}
	if(! m_LODDesc->IsConsistentWithPatch( this ) )
	{
		RecalcIndexBuffer();
	}
	SynchronizeDescWithState();
}

void TerrainPatch::RecalcIndexBuffer()
{
	int level = (int) m_LOD;
	SetIndexSimplification();
	m_mipLevels[level ]->UpdateIndices();
}

void TerrainPatch::Refresh()
{
	/*for( unsigned int i=0; i<m_mipLevels.size(); i++)
	{
		m_mipLevels[i]->InitVertexHeights();
	}*/

}

void TerrainPatch::Draw()
{
	DrawLevel(m_LOD);
}

void TerrainPatch::DrawLevel( float LOD )
{
	/*m_screen.Push();
	m_screen.LoadMatrix( Matrix.Translation( this.Offset ) );

	int level = (int)LOD;
	float interpolation = LOD-level;
	m_mipLevels[level].Interpolation = interpolation;
	m_mipLevels[level].Draw();

	m_screen.Pop();*/

	m_screen->SetTransform( Matrix::Translation(m_offset.x, m_offset.y, m_offset.z));
	int level = (int)m_LOD;
	float interpolation = m_LOD-level;
	m_mipLevels[level]->Interpolation( interpolation);
	m_mipLevels[level]->Draw();

}