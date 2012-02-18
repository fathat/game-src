#pragma once

class TerrainIndexBlock
{
	int m_size;
	unsigned short* m_indices;

public:
	int m_westSimplificationLevel; //=1
	int m_eastSimplificationLevel;
	int m_northSimplificationLevel;
	int m_southSimplificationLevel;


	TerrainIndexBlock(int dim);
	~TerrainIndexBlock();


	int IndexCount();
	int TriangleCount();
	
	unsigned short* Indices() ;
	
	/// <summary>
	/// Gives back an index number for the specified coordinate.
	/// </summary>
	/// <param name="x">Column</param>
	/// <param name="y">Row</param>
	/// <returns>Returns an index into a vertex buffer.</returns>
	int GetIndexForCoordinate( int x, int y);


	/// <summary>
	/// Allocates space for the indices and sets them.
	/// </summary>
	void MakeBasicIndices();

	/// <summary>
	/// Updates the indices with the correct values.
	/// </summary>
	void UpdateIndices( );


};