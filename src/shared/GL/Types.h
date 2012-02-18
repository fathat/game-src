#ifndef __3DMATH_H__
#define __3DMATH_H__

// I N C L U D E S /////////////////////////////////////////////////////

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <math.h>
#include <string>


// P R O T O T Y P E S /////////////////////////////////////////////////

void OutputMessage(char* text, ...);
void OutputDXError(HRESULT Error, char* text);
std::string GetRelativeDirectory( char* WorkingDirectory, char* In );

class Screen3D;


// D E F I N E S ///////////////////////////////////////////////////////

#ifndef TextureHandle
#define TextureHandle int
#endif

enum CollisionCheckType { Box, Sphere, Normal };



#define Real float
#define PI 3.141592653589793F
#define RADIAN(x) ((x)*PI/180.0f)
#define DEGREE(x) ((x)*180.0f/PI)


//types of polygon (used by Polygon structure)
#define S3DPT_TRIANGLELIST  D3DPT_POINTLIST
#define S3DPT_TRIANGLESTRIP D3DPT_LINELIST
#define S3DPT_LINELIST      D3DPT_LINESTRIP
#define S3DPT_POINTLIST     D3DPT_TRIANGLELIST
#define S3DPT_LINESTRIP     D3DPT_TRIANGLESTRIP
#define S3DPT_TRIANGLEFAN   D3DPT_TRIANGLEFAN

#define S3DPOLYGON_FVF (D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_TEX1) 
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL |D3DFVF_DIFFUSE|D3DFVF_TEX1)



// C L A S S E S ///////////////////////////////////////////////////////


struct Matrix
{
	D3DXMATRIX Mat;

	Matrix() { Identity(); }

	Matrix operator + ( const Matrix& m );
	Matrix operator - ( const Matrix& m );
	Matrix operator * ( Matrix& m ) ;
	void   operator = ( const Matrix& m ) { Mat = m.Mat; }

	void Identity()
	{
		D3DXMatrixIdentity(&Mat);
		
	}

	void Translate ( Real x, Real y, Real z)
	{
		D3DXMatrixTranslation(&Mat, x, y, z);
	}

	void Rotate(Real Yaw, Real Pitch, Real Roll)
	{
		D3DXMatrixRotationYawPitchRoll(&Mat, Yaw, Pitch, Roll);
	}

	void RotateYaw   ( Real Rad ) { D3DXMatrixRotationY(&Mat, Rad); }
	void RotatePitch ( Real Rad ) { D3DXMatrixRotationX(&Mat, Rad); }
	void RotateRoll  ( Real Rad ) { D3DXMatrixRotationZ(&Mat, Rad); }
	
};

struct Vector3D;

struct Point3D {
   Real x, y, z;

   void Rotatex ( Real Rad );

   void Rotatey ( Real Rad );

   void Rotatez ( Real Rad );

   operator Vector3D  ();

   Point3D operator - ( Point3D &P );

   Point3D operator + ( Point3D &P );

   Point3D operator * ( Real S );

   Real DistanceTo ( Point3D &P );
};

struct Vector3D {
   Real x, y, z;

   void Rotatex ( Real Rad );

   void Rotatey ( Real Rad );

   void Rotatez ( Real Rad );

   Real Dot ( Vector3D &V );

   Vector3D Cross ( Vector3D &V );

   Real Mag ();

   Real Angle ( Vector3D &V );
   Real AngleNorm ( Vector3D &V );

   operator Real ();
   operator Point3D ();
   
   Vector3D &Normalize ();

   Vector3D operator - ( Vector3D &V );
   Vector3D operator + ( Vector3D &V );
   Vector3D operator * ( Real S );
};


struct Quaternion
{
	D3DXQUATERNION Quat;

	Quaternion() { D3DXQuaternionIdentity(&Quat); }
	

	void Invert() { 
		D3DXQUATERNION SourceQ = Quat;
		D3DXQuaternionInverse( &Quat, &SourceQ);
	}

	Quaternion Slerp( Quaternion& Q, Real t) {
		
		Quaternion ReturnQuat;
		D3DXQuaternionSlerp(&ReturnQuat.Quat, &Quat, &Q.Quat, t);

		return ReturnQuat;
	}

	void Rotate(Real Yaw, Real Pitch, Real Roll) {
		D3DXQuaternionRotationYawPitchRoll(&Quat, Yaw, Pitch, Roll);
	}


	Quaternion operator * ( Quaternion Q );
	void operator = ( Matrix& Mat);

};


struct MeshMaterial
{
	//constructors
	MeshMaterial (); 
	~MeshMaterial ();

	//color stuff
	D3DCOLORVALUE AmbientRGBA;
	D3DCOLORVALUE DiffuseRGBA;
	D3DCOLORVALUE SpecularRGBA;
	Real Transparency;
	Real Shininess; //how much specular lighting there is.

	//texture stuff
	TextureHandle Image;
};




// A structure for our custom vertex type. 
struct CustomVertex
{
    D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;   // Normal
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates

	CustomVertex() { 
		ZeroMemory(this, sizeof(CustomVertex));
	}
};




struct Triangle3D
{
	//pointer to the list of vertices where this
	//triangle gets its data
	CustomVertex* SourceVertexList;
	int nVertices;

	//indices in that list that this triangle uses
	int Index[3];
	
	//material for this triangle	
	MeshMaterial Mat;


	Triangle3D(CustomVertex* list, int ListSize, int index1=-1, int index2=-1, int index3=-1)  {
		SourceVertexList = list;
		nVertices  = ListSize;
		Index[0]   = index1;
		Index[1]   = index2;
		Index[2]   = index3;
	}
	
	Triangle3D() { 
		ZeroMemory(this, sizeof(Triangle3D));
	
	} 
};



struct Rect2D
{
	Real x;
	Real y;
	Real Width;
	Real Height;

	Rect2D() { ZeroMemory(this, sizeof(Rect2D)); }
};



struct Position
{
	Real x, y, z;
	Real Yaw, Pitch, Roll;

	Matrix GetMatrix();
	Position() { ZeroMemory(this, sizeof(Position)); }
};


struct Camera : public Position
{
	void Follow( Position& Location, Real Distance, Real Pitch=-1 );

	Matrix GetLookAtMatrix( Real Distance );
	Matrix GetViewMatrix( );
};

struct BoundingVolume : public Position
{
//virtual CalcBoundingVolume()
};


struct BoundingBox : public BoundingVolume
{

Point3D BoxCoords[8];
};

struct BoundingSphere : public BoundingVolume
{
Real Radius;
};

struct BoundingCube : public BoundingVolume
{
Real Size; 
};

struct AABB 
{
	D3DXVECTOR3 AABBmin;
	D3DXVECTOR3 AABBmax;

	
	//hacks!
	bool SphereContained( BoundingSphere &Sphere )
	{
		//Hack! >_< !
		if(Sphere.x <= AABBmax.x &&
		   Sphere.z <= AABBmax.z &&
		   Sphere.x >= AABBmin.x &&
		   Sphere.z >= AABBmin.z )

		   return true;

		return false;
		
	}

	bool BoxContained( AABB& Box )
	{
		Vector3D Point;

		Point.x = Box.AABBmin.x;
		Point.z = Box.AABBmin.z;
		if(PointContained( Point )) return true;

		Point.x = Box.AABBmax.x;
		Point.z = Box.AABBmin.z;
		if(PointContained( Point )) return true;

		Point.x = Box.AABBmin.x;
		Point.z = Box.AABBmax.z;
		if(PointContained( Point )) return true;

		Point.x = Box.AABBmax.x;
		Point.z = Box.AABBmax.z;
		if(PointContained( Point )) return true;



		return false;
		
	}
	bool PointContained( Vector3D& Point)
	{

		if(Point.x <= AABBmax.x &&
		   Point.z <= AABBmax.z &&
		   Point.x >= AABBmin.x &&
		   Point.z >= AABBmin.z )

		   return true;

		return false;

	}

	AABB InterpolateBox( AABB& IntBox, Real Percent) {
		AABB ResultBox;
		Real InvPercent = 1.0f-Percent;
		ResultBox.AABBmin.x = (AABBmin.x * InvPercent) + (IntBox.AABBmin.x * Percent);
		ResultBox.AABBmin.y = (AABBmin.y * InvPercent) + (IntBox.AABBmin.y * Percent);
		ResultBox.AABBmin.z = (AABBmin.z * InvPercent) + (IntBox.AABBmin.z * Percent);
		ResultBox.AABBmax.x = (AABBmax.x * InvPercent) + (IntBox.AABBmax.x * Percent);
		ResultBox.AABBmax.y = (AABBmax.y * InvPercent) + (IntBox.AABBmax.y * Percent);
		ResultBox.AABBmax.z = (AABBmax.z * InvPercent) + (IntBox.AABBmax.z * Percent);
        return ResultBox;
	}
};

#endif
