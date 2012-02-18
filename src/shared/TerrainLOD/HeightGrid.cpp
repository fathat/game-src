#include "HeightGrid.h"

int HeightGrid::Size() { 
	return m_size; 
}
int HeightGrid::SquaresAcross() {
	return m_size-1; 
}

int HeightGrid::HalfSize()
{
	return (m_size-1)/2 + 1;
}


HeightGrid::HeightGrid( int dim )
{
	Init ( dim );
}

HeightGrid::~HeightGrid()
{
	delete []m_heights;
}

/// <summary>
/// Initializes stuff.
/// </summary>
/// <param name="dim"></param>
void HeightGrid::Init( int dim )
{
	m_heights = new float[dim*dim];
	m_size    = dim;
	m_highestHeight  = 0;
	m_lowestHeight   = 0;

	for( int i=0; i<m_size*m_size; i++ )
		m_heights[i] = 0;
}

/// <summary>
/// Sets.. a height! At a coordinate!
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="newHeight"></param>
void HeightGrid::SetHeight( int x, int y, float newHeight )
{
	m_heights[x+y*m_size ] = newHeight;

	m_lowestHeight = MIN( m_lowestHeight, newHeight );
	m_highestHeight = MAX( m_highestHeight , newHeight );


}

/// <summary>
/// Returns the height value at a given location. Note that this function
/// wraps, so if x is greater than the size it'll wrap back to the beginning
/// and go from there.
/// </summary>
/// <param name="x">x coordinate</param>
/// <param name="y">y coordinate</param>
/// <returns>Returns a height.</returns>
float HeightGrid::GetHeight( int x, int y )
{
	x = x % m_size;
	y = y % m_size;
	return m_heights[x+y*m_size ];
}

/// <summary>
/// Finds lowest and highest height.
/// </summary>
void HeightGrid::CalculateHeightExtremities () 
{
	m_lowestHeight = m_heights[0];
	m_highestHeight= m_heights[0];

	for( int i= 0; i<m_size*m_size; i++)
	{

		m_lowestHeight = MIN( m_heights[i], m_lowestHeight );
		m_highestHeight = MAX( m_heights[i], m_highestHeight );
	}
}


/// <summary>
/// Calculates the triangle at a given position on the grid. 
/// </summary>
/// <param name="x">x coordinate (can be between two grid locations)</param>
/// <param name="y">y coordinate (can be between two grid locations)</param>
/// <returns>Returns the 3 vertices of the triangle</returns>
TerrainTriangle HeightGrid::GetTriangleAtPosition (float x, float y)
{
	Vector3D list[3];
	int ix, iy;
	ix = (int) x;
	iy = (int) y;
	float balance = (x-ix) + (y-iy);
	if( balance < 1 )
	{
		list[0] = Vector3D((float) ix,   (float)GetHeight( ix, iy ),  (float)iy );
		list[1] = Vector3D((float) ix,   (float)GetHeight( ix, iy+1), (float)iy+1 );
		list[2] = Vector3D((float) ix+1, (float)GetHeight( ix+1, iy), (float)iy );		
	}	
	else
	{
		list[0] = Vector3D((float) ix,   (float)GetHeight( ix, iy+1),   (float)iy+1 );
		list[1] = Vector3D((float) ix+1, (float)GetHeight( ix+1, iy+1), (float)iy+1 );	
		list[2] = Vector3D((float) ix+1, (float)GetHeight( ix+1, iy),   (float)iy );	

	}
	return TerrainTriangle( list ); 

}

/// <summary>
/// Works like GetHeight, only you can specify a coordinate between two grid points
/// to get an interpolated position.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
float HeightGrid::GetInterpolatedHeight( float x, float y )
{

	D3DXPLANE plane;
	TerrainTriangle t = GetTriangleAtPosition( x, y );
	plane = t.GetPlane();

	Vector3D intersection;
	Vector3D l1 = Vector3D( x, 1000 , y );
	Vector3D l2 = Vector3D( x, -1000, y );


	D3DXPlaneIntersectLine( (D3DXVECTOR3*)&intersection, &plane, (const D3DXVECTOR3*)&l1,(const D3DXVECTOR3*) &l2 );
	//intersection = Plane.IntersectLine( plane, l1, l2 );

	//terrainnormal = new DX.Vector3D( trianglePlane.A,trianglePlane.B,trianglePlane.C );

	//return the y value
	return intersection.y;

}



Vector3D HeightGrid::GetNormal( float x, float y )
{
	TerrainTriangle t = GetTriangleAtPosition ( x, y );
	D3DXPLANE p = t.GetPlane();
	Vector3D rval = Vector3D( p.a, p.b, p.c );
	rval.Normalize();
	return rval;
}


/// <summary>
/// Returns the 2 Triangles that form the quad at the given grid location. 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
TerrainTriangle* HeightGrid::GetTrianglesAtGridLocation( int x, int y )
{
	int ix, iy;
	ix = (int) x;
	iy = (int) y;

	TerrainTriangle* list = new TerrainTriangle[2];

	list[0] = TerrainTriangle(
		Vector3D( (float)ix,   (float)GetHeight( ix, iy ),  (float)iy ),
		Vector3D( (float)ix,   (float)GetHeight( ix, iy+1), (float)iy+1 ),
		Vector3D( (float)ix+1, (float)GetHeight( ix, iy),   (float)iy ) );		



	list[1] = TerrainTriangle(
		Vector3D( (float)ix,   (float)GetHeight( ix, iy+1),   (float)iy+1 ),
		Vector3D( (float)ix+1, (float)GetHeight( ix+1, iy+1), (float)iy+1 ),
		Vector3D( (float)ix+1, (float)GetHeight( ix+1, iy),   (float)iy ) );	

	return list; 

}


/// <summary>
/// Returns all triangles that this HeightGrid forms.
/// </summary>
/// <returns>Returns an array of triangles.</returns>
TerrainTriangle* HeightGrid::GetAllTriangles()
{
	int tAcross = m_size-1;
	TerrainTriangle* triangles = new TerrainTriangle[tAcross*tAcross * 2];

	int i=0;
	for( int y=0; y< tAcross; y++ )
	{
		for( int x=0; x< tAcross; x++ )
		{
			TerrainTriangle* t;
			t = GetTrianglesAtGridLocation( x, y );
			triangles[i] = t[0];
			triangles[i+1] = t[1];
			i += 2;
		}
	}

	return triangles;
}

HeightGrid* HeightGrid::HalfLOD()
{
	//makes a new heightgrid based on this one, only with half the resolution
	//Why doesn't this just do m_size/2? Well, because if we're dividing a height grid
	//it probably has a size of 2^n + 1, so doing this will give us 2^(n-1) +1
	HeightGrid* hg = new HeightGrid( (m_size-1)/2 + 1);

	for( int y=0, hy=0; hy<hg->Size(); hy++, y+=2  )
	{
		for( int x=0, hx=0; hx<hg->Size(); hx++, x+=2 )	
		{
			hg->SetHeight( hx, hy, GetHeight( x, y ) );
		}
	}

	return hg;

}

HeightGrid* HeightGrid::DoubleLOD()
{
	HeightGrid* hg = new HeightGrid ( (m_size-1) * 2 + 1) ;

	for( int y=0; y < hg->Size(); y++  )
	{
		for( int x=0; x<hg->Size(); x++ )	
		{
			hg->SetHeight( x, y, GetInterpolatedHeight( (float)x/2.0f, (float)y/2.0f ) );
		}
	}

	return hg;
}


HeightGrid* HeightGrid::Duplicate()
{
	HeightGrid* hg = new HeightGrid ( m_size );  
	for( int y=0; y < hg->Size(); y++  ) {
		for( int x=0; x<hg->Size(); x++ ) {
			hg->SetHeight( x, y, GetHeight( x, y ) );
		}
	}
	return hg;
}


/// <summary>
/// Makes a height grid suitable as an interpolation target
/// when doing geomipmapping.
/// </summary>
/// <returns>Returns this height grid as an imitation of a lower resolution LOD.</returns>
HeightGrid* HeightGrid::MakeSimplificationTarget()
{
	//This function reduces the resolution by half, then
	//rescales the grid back to its original size.
	//The idea here is that the simplification target needs
	//to be at the same resolution as the regular height grid,
	//but it should *look* like the lower resolution version.
	HeightGrid* hlod = HalfLOD();
	HeightGrid* dlod = hlod->DoubleLOD();
	delete hlod;
	return dlod;

}

/// <summary>
/// Grabs as subsection of this HeightGrid and makes an object for it.
/// </summary>
/// <param name="startx">x coordinate to start at</param>
/// <param name="starty">y coordinate to start at</param>
/// <param name="size">size of the box being grabbed (height grids are always square)</param>
/// <returns>Returns a new height grid.</returns>
HeightGrid* HeightGrid::MakeHeightGridFromSubset( int startx, int starty, int size )
{
	HeightGrid* hg = new HeightGrid( size );
	for (int y=0; y<size; y++ )
	{
		for (int x=0; x<size; x++ )
		{
			hg->SetHeight( x, y, GetHeight( x+startx, y+starty ) );
		}
	}
	hg->CalculateHeightExtremities () ;
	return hg;
}

/// <summary>
/// Subdivides the grid into 4 quads. Note that the array ordering is
/// "top left, top right, bottom left, bottom right".
/// </summary>
/// <returns>Returns an array of 4 new height grids.</returns>
HeightGrid** HeightGrid::SubdivideIntoQuads()
{	
	int halfSize=HalfSize();
	HeightGrid** gridList = new HeightGrid*[4];

	gridList[0] = MakeHeightGridFromSubset( 0, 0, halfSize );
	gridList[1] = MakeHeightGridFromSubset( halfSize-1, 0, halfSize );
	gridList[2] = MakeHeightGridFromSubset( 0, halfSize-1, halfSize );
	gridList[3] = MakeHeightGridFromSubset( halfSize-1, halfSize-1, halfSize );
	return gridList;

}


HeightGrid** HeightGrid::Fracture(int dim )
{
	HeightGrid** gridList = new HeightGrid*[dim*dim];
	int patchSize = (this->Size()-1)/ dim + 1 ; 

	for( int y=0; y<dim; y++ )
	{
		for (int x=0; x<dim; x++)
		{
			gridList[x+y*dim] = MakeHeightGridFromSubset( x*(patchSize-1), y*(patchSize-1), patchSize  );
		}
	}
	return gridList;
}
