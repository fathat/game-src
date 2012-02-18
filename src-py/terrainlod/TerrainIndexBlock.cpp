#include "TerrainIndexBlock.h"

int TerrainIndexBlock::IndexCount() {
	return TriangleCount()*3;
}
int TerrainIndexBlock::TriangleCount() {
	return (m_size-1)*(m_size-1)*2;  
}

unsigned short* TerrainIndexBlock::Indices() {
	return m_indices;  
}


TerrainIndexBlock::TerrainIndexBlock(int dim)
{
	m_westSimplificationLevel=1;
	m_eastSimplificationLevel=1;
	m_northSimplificationLevel=1;
	m_southSimplificationLevel=1;
	m_size = dim;
	MakeBasicIndices();
}

TerrainIndexBlock::~TerrainIndexBlock()
{
	delete m_indices;
}


/// <summary>
/// Gives back an index number for the specified coordinate.
/// </summary>
/// <param name="x">Column</param>
/// <param name="y">Row</param>
/// <returns>Returns an index into a vertex buffer.</returns>
int TerrainIndexBlock::GetIndexForCoordinate( int x, int y)
{
	int index;

	if( m_size > 2 )
	{

		if( x==0 && m_westSimplificationLevel > 1 ) {
			y-= y%m_westSimplificationLevel; 
		}
		if( x==m_size-1  &&y%m_eastSimplificationLevel != 0) {
			y+= (m_eastSimplificationLevel-(y%m_eastSimplificationLevel)); 
		}

		if( y==0  && m_northSimplificationLevel > 1) {
			x-= x%m_northSimplificationLevel; 
		}
		if( y==m_size-1  && x%m_southSimplificationLevel != 0 ) {
			x+= (m_southSimplificationLevel-(x%m_southSimplificationLevel)); 
		}
	}

	index =  x+y*m_size;
	return index;
}


/// <summary>
/// Allocates space for the indices and sets them.
/// </summary>
void TerrainIndexBlock::MakeBasicIndices()
{
	//allocate the memory
	int nTriangles = (m_size-1)*(m_size-1)*2;
	m_indices = new unsigned short[nTriangles*3];

	UpdateIndices();

}

/// <summary>
/// Updates the indices with the correct values.
/// </summary>
void TerrainIndexBlock::UpdateIndices( )
{
	//make basic indices
	int index = 0;
	for( int y=0; y<m_size-1; y++ )
	{
		for( int x=0; x<m_size-1; x++ )
		{

			//triangle 1
			m_indices[index+0] = GetIndexForCoordinate( x, y );
			m_indices[index+1] = GetIndexForCoordinate( x, y+1 );
			m_indices[index+2] = GetIndexForCoordinate( x+1, y );

			//triangle 2
			m_indices[index+3] = GetIndexForCoordinate( x, y+1 );
			m_indices[index+4] = GetIndexForCoordinate( x+1, y+1 );
			m_indices[index+5] = GetIndexForCoordinate( x+1, y );
			index += 6;
		}
	}	
}