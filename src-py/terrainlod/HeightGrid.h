#pragma once
#include <Python.h>
#include "MathTypes.h"

struct TerrainTriangle
{
	Vector3D v1, v2 , v3;


	Plane GetPlane();

	TerrainTriangle( Vector3D* list )
	{
		v1 = list[0];
		v2 = list[1];
		v3 = list[2];
	}

	TerrainTriangle( Vector3D a, Vector3D b, Vector3D c)
	{
		v1 = a;
		v2 = b;
		v3 = c;
	}

	TerrainTriangle()
	{

	}
};


struct TrianglePair 
{
	TerrainTriangle a;
	TerrainTriangle b;

	TerrainTriangle A() { return a; }
	TerrainTriangle B() { return b; }

	TrianglePair( TerrainTriangle _a, TerrainTriangle _b )
	{
		a = _a;
		b = _b;
	}
};



/// <summary>
/// Summary description for HeightGrid.
/// </summary>
class HeightGrid
{

	//word
	float*  m_heights ;
	int		m_size	  ;

	float m_lowestHeight ;
	float m_highestHeight ;


public:
	int Size() ;
	int SquaresAcross();
	int HalfSize();

	float HighestHeight() { return m_highestHeight; }
	float LowestHeight() { return m_lowestHeight; }
	PyObject* HeightData();

	HeightGrid( PyObject* list);
	HeightGrid( int dim );
	~HeightGrid();

	/// <summary>
	/// Initializes stuff.
	/// </summary>
	/// <param name="dim"></param>
	void Init( int dim );

	/// <summary>
	/// Sets.. a height! At a coordinate!
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="newHeight"></param>
	void SetHeight( int x, int y, float newHeight );

	/// <summary>
	/// Returns the height value at a given location. Note that this function
	/// wraps, so if x is greater than the size it'll wrap back to the beginning
	/// and go from there.
	/// </summary>
	/// <param name="x">x coordinate</param>
	/// <param name="y">y coordinate</param>
	/// <returns>Returns a height.</returns>
	float GetHeight( int x, int y );

	/// <summary>
	/// Finds lowest and highest height.
	/// </summary>
	void CalculateHeightExtremities () ;


	/// <summary>
	/// Calculates the triangle at a given position on the grid. 
	/// </summary>
	/// <param name="x">x coordinate (can be between two grid locations)</param>
	/// <param name="y">y coordinate (can be between two grid locations)</param>
	/// <returns>Returns the 3 vertices of the triangle</returns>
	TerrainTriangle GetTriangleAtPosition (float x, float y);

	PyObject* GetNormalAtPosition(float tx, float tz);

	/// <summary>
	/// Works like GetHeight, only you can specify a coordinate between two grid points
	/// to get an interpolated position.
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	float GetInterpolatedHeight( float x, float y );


	/// <summary>
	/// Returns the 2 Triangles that form the quad at the given grid location. 
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	TrianglePair GetTrianglesAtGridLocation( int x, int y );


	/// <summary>
	/// Returns all triangles that this HeightGrid forms.
	/// </summary>
	/// <returns>Returns an array of triangles.</returns>
	TerrainTriangle* GetAllTriangles();

	HeightGrid* HalfLOD();
	HeightGrid* DoubleLOD();

	HeightGrid* Duplicate();

	/// <summary>
	/// Makes a height grid suitable as an interpolation target
	/// when doing geomipmapping.
	/// </summary>
	/// <returns>Returns this height grid as an imitation of a lower resolution LOD.</returns>
	HeightGrid* MakeSimplificationTarget();

	/// <summary>
	/// Grabs as subsection of this HeightGrid and makes an object for it.
	/// </summary>
	/// <param name="startx">x coordinate to start at</param>
	/// <param name="starty">y coordinate to start at</param>
	/// <param name="size">size of the box being grabbed (height grids are always square)</param>
	/// <returns>Returns a new height grid.</returns>
	HeightGrid* MakeHeightGridFromSubset( int startx, int starty, int size );

	/// <summary>
	/// Subdivides the grid into 4 quads. Note that the array ordering is
	/// "top left, top right, bottom left, bottom right".
	/// </summary>
	/// <returns>Returns an array of 4 new height grids.</returns>
	HeightGrid** SubdivideIntoQuads();

	HeightGrid** Fracture( int dim );


};