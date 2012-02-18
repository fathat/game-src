#ifndef __3DMATH_H__
#define __3DMATH_H__

// I N C L U D E S /////////////////////////////////////////////////////

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>


// P R O T O T Y P E S /////////////////////////////////////////////////

void OutputMessage(char* text, ...);
void OutputDXError(HRESULT Error, char* text);

class Screen3D;



// D E F I N E S ///////////////////////////////////////////////////////
	
//#pragma warning( disable : 4018 )
//#pragma warning( disable : 4244 )

#define GetBit(bits, bitposition) (((1<<bitposition)&bits)>>bitposition)
#define SetBit(bits,bit, bitposition) (bits|=(bit<<bitposition))


#define MAX( a, b ) ( ( a > b ) ? (a) : (b) )
#define MIN( a, b ) ( ( a < b ) ? (a) : (b) )


#ifndef TextureHandle
#define TextureHandle int
#endif


//disable warning 4786, it's unreadable anyway
//#pragma warning ( disable :4786 )

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


// F U N C T I O N S ////////////////////////////////////////////////////
extern float	(*m_atan)(float);
extern float	(*m_atan2)(float,float);
extern float	(*m_acos)(float);
extern float	(*m_asin)(float);
extern float  (*m_log)(float);
extern float  (*m_log10)(float);
extern float  (*m_pow)(float, float);
extern float	(*m_exp)(float);
extern float  (*m_sqrt)(float);
extern float 	(*m_fabs)(float);
extern float  (*m_ceil)(float);
extern float 	(*m_floor)(float);
extern float 	(*m_frexp)(float, int *);
extern float	(*m_ldexp)(float, int);
extern float  (*m_modf)(float, float *);
extern float	(*m_fmod)(float, float);
extern void 	(*m_sincos)(float, float *);
extern float	(*m_sin)(float);
extern float	(*m_cos)(float);
extern float  (*m_tan)(float);


float	s_atan(float);
float	s_atan2(float,float);
float	s_acos(float);
float	s_asin(float);
float	s_log(float);
float	s_log10(float);
float	s_pow(float, float);
float	s_exp(float);
float	s_sqrt(float);
float 	s_fabs(float);
float	s_ceil(float);
float 	s_floor(float);
float 	s_frexp(float, int *);
float	s_ldexp(float, int);
float	s_modf(float, float *);
float	s_fmod(float, float);
void 	s_sincos(float, float *);
float	s_sin(float);
float	s_cos(float);
float	s_tan(float);

Real CutValue( Real Value, Real amount );



// C L A S S E S ///////////////////////////////////////////////////////


struct Matrix
{

	//static functions
	static Matrix Translation( float x, float y, float z )
	{
		Matrix m;
		m.Translate(x, y, z );
		return m;
	}

	static Matrix MakeIdentity()
	{
		Matrix m;
		return m;
	}

	D3DXMATRIX Mat;

	Matrix() { Identity(); }

	Matrix operator + ( const Matrix& m ) const;
	Matrix operator - ( const Matrix& m ) const;
	Matrix operator * ( Matrix& m )		  const;
	void   operator = ( const Matrix& m ) { Mat = m.Mat; }

	void Identity()
	{
		D3DXMatrixIdentity(&Mat);
		
	}

	void Invert()
	{
		D3DXMATRIX inverse;
		D3DXMatrixInverse( &inverse, NULL, &Mat );
		Mat = inverse;
	}

	void Translate ( Real x, Real y, Real z)
	{
		D3DXMatrixTranslation(&Mat, x, y, z);
	}

	void Rotate(Real Yaw, Real Pitch, Real Roll)
	{
		D3DXMatrixRotationYawPitchRoll(&Mat, Yaw, Pitch, Roll);
	}

	void Scale( Real x, Real y, Real z )
	{
		D3DXMatrixScaling( &Mat, x, y, z);
	}

	void RotateYaw   ( Real Rad ) { D3DXMatrixRotationY(&Mat, Rad); }
	void RotatePitch ( Real Rad ) { D3DXMatrixRotationX(&Mat, Rad); }
	void RotateRoll  ( Real Rad ) { D3DXMatrixRotationZ(&Mat, Rad); }
	
};

struct Vector3D;


struct Plane3D
{
	
	D3DXVECTOR3 Normal;
	Real Distance;
	
	Real a() { return Normal.x; }
	Real b() { return Normal.y; }
	Real c() { return Normal.z; }
	Real d() { return Distance; }
	void a( Real v ) { Normal.x = v; }
	void b( Real v ) { Normal.y = v; }
	void c( Real v ) { Normal.z = v; }
	void d( Real v ) { Distance  = v; }

	Plane3D()
	{
		Normal = D3DXVECTOR3( 0, 0 , 0 );
		Distance = 0;
	}

	Plane3D ( float a, float b, float c, float d )
	{
		Normal.x = a;
		Normal.y = b;
		Normal.z = c;
		Distance = d;
	}

	Real DistanceToPoint(D3DXVECTOR3 Point){return ((Point.x*Normal.x)+(Point.y*Normal.y)+(Point.z*Normal.z)+Distance);}
};


struct SoundSource
{
	float x, y, z;
	float vx, vy, vz;
	float ox, oy, oz;

	SoundSource( float _x, float _y, float _z, 
				 float _vx, float _vy, float _vz, 
				 float _ox=0, float _oy=0,float  _oz=0 ):
	x(_x),
		y(_y),
		z(_z),
		vx(_vx),
		vy(_vy),
		vz(_vz),
		ox(_ox),
		oy(_oy),
		oz(_oz)
	{
	}
};


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

	//Real x, y, z;
	union
	{
		float x;
		float yaw;
	};
	union
	{
		float y;
		float pitch;
	};

	union
	{
		float z;
		float roll;
	};



	Vector3D() { x = 0; y = 0; z = 0; }
	Vector3D( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; }
	Vector3D( float* v ) { x = v[0]; y = v[1]; z = v[2]; }
	Vector3D( double* v ) { x = (float)v[0]; y = (float)v[1]; z = (float)v[2]; }

	void Rotatex ( Real Rad );
	void Rotatey ( Real Rad );
	void Rotatez ( Real Rad );

	float CutLength( float amount );
	void  SetLength( float l );


	void   operator = ( const D3DXVECTOR3& m ) { x=m.x; y=m.y; z=m.z;  }

	Real Dot ( Vector3D &V );
	Vector3D Cross ( Vector3D &V );

	Real Mag () const;
	Real MagSq() const;
	Real Length() { return Mag(); }
	Real LengthSq() { return MagSq(); }

	Real Angle ( Vector3D &V );
	Real AngleNorm ( Vector3D &V );

	Vector3D Reflect( Vector3D& Normal );

	operator Real ();
	operator Point3D ();
	operator D3DXVECTOR3();

	void Transform( Matrix& matrix );


	Vector3D &Normalize ();

	float& operator [] ( int index );
	//void  operator [] ( int index, float value );

	Vector3D operator - ( const Vector3D &V ) const;
	Vector3D operator + ( const Vector3D &V ) const;
	Vector3D operator * ( const Vector3D &V ) const;
	Vector3D operator / ( const Vector3D &V ) const;
	void     operator += ( const Vector3D &V ) ;
	void     operator -= ( const Vector3D &V ) ;
	Vector3D operator *  ( const Real S ) const;
	Vector3D operator /  ( const Real S ) const;
	void     operator *= ( const Real S );
	void     operator /= ( const Real S );
	void     operator += ( const Real S );
	void     operator -= ( const Real S );

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




struct Rect2D
{
	Real left;
	Real top;
	Real right;
	Real bottom;

	Rect2D() { ZeroMemory(this, sizeof(Rect2D)); }
};



struct Position
{
	Real x, y, z;

	Real Yaw, Pitch, Roll;
	Matrix GetMatrix();
	Matrix GetRotationMatrix();

	Position ( Vector3D& location )
	{
		x = location.x;
		y = location.y;
		z = location.z;
		Yaw   = 0;
		Pitch = 0;
		Roll  = 0;
	}

	Position ( Vector3D& location, Vector3D& rotation )
	{
		x = location.x;
		y = location.y;
		z = location.z;
		Yaw   = rotation.x ;
		Pitch = rotation.y ;
		Roll  = rotation.z ;
	}

	void operator = ( Vector3D& newLocation ) { 
		x = newLocation.x;
		y = newLocation.y;
		z = newLocation.z;
	}

	void SetLocation( Vector3D& v  ) {
		x = v.x;
		y = v.y;
		z = v.z;
	}

	void SetRotation( Vector3D& v ) {
		Yaw   = v.x;
		Pitch = v.y;
		Roll  = v.z;
	}

	operator Vector3D ();

	Vector3D GetVector () 
	{
		return Vector3D(x, y, z );
	}

	void MoveBy( Vector3D& offset )
	{
		x += offset.x;
		y += offset.y;
		z += offset.z;
	}

	SoundSource AsSoundSource();

	Position() { ZeroMemory(this, sizeof(Position)); }
};



struct BoundingVolume 
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
	Vector3D location;

	void x( float val ) { location.x = val ; }
	void y( float val ) { location.y = val ; }
	void z( float val ) { location.z = val ; }
	float x() { return location.x;}
	float y() { return location.y;}
	float z() { return location.z;}

};

struct BoundingCube : public BoundingVolume
{
	float x, y, z;
	Real Size; 
};

struct AABB 
{
	Vector3D min;
	Vector3D max;

	
	AABB() {};
	AABB( Vector3D a, Vector3D b )
	{
		min = a;
		max = b;
	}
	AABB( float x1, float y1, float z1, float x2, float y2, float z2 )
	{
		min.x = x1;
		min.y = y1;
		min.z = z1;

		max.x = x2;
		max.y = y2;
		max.z = z2;
	}
	/*bool PointContained( Vector3D& Point)
	{

		if(Point.x <= max.x &&
		   Point.z <= max.z &&
		   Point.x >= min.x &&
		   Point.z >= min.z )

		   return true;

		return false;

	}*/

	AABB InterpolateBox( AABB& IntBox, Real Percent) {
		AABB ResultBox;
		Real InvPercent = 1.0f-Percent;
		ResultBox.min.x = (min.x * InvPercent) + (IntBox.min.x * Percent);
		ResultBox.min.y = (min.y * InvPercent) + (IntBox.min.y * Percent);
		ResultBox.min.z = (min.z * InvPercent) + (IntBox.min.z * Percent);
		ResultBox.max.x = (max.x * InvPercent) + (IntBox.max.x * Percent);
		ResultBox.max.y = (max.y * InvPercent) + (IntBox.max.y * Percent);
		ResultBox.max.z = (max.z * InvPercent) + (IntBox.max.z * Percent);
        return ResultBox;
	}
};

#endif