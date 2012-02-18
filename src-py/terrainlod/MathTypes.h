#ifndef MATH_H
#define MATH_H

// I N C L U D E S /////////////////////////////////////////////////////

// P R O T O T Y P E S /////////////////////////////////////////////////

// D E F I N E S ///////////////////////////////////////////////////////

#define GetBit(bits, bitposition) (((1<<bitposition)&bits)>>bitposition)
#define SetBit(bits,bit, bitposition) (bits|=(bit<<bitposition))

#define MAX( a, b ) ( ( a > b ) ? (a) : (b) )
#define MIN( a, b ) ( ( a < b ) ? (a) : (b) )

#define Real float
#define PI 3.141592653589793F
#define RADIAN(x) ((x)*PI/180.0f)
#define DEGREE(x) ((x)*180.0f/PI)

//Real CutValue( Real Value, Real amount );

// C L A S S E S ///////////////////////////////////////////////////////

/*struct Matrix
{

	//static functions
	static Matrix Translation( float x, float y, float z );
	static Matrix MakeIdentity();

	Matrix() { Identity(); }

	Matrix operator + ( const Matrix& m ) const;
	Matrix operator - ( const Matrix& m ) const;
	Matrix operator * ( Matrix& m )		  const;
	void   operator = ( const Matrix& m );

	void Identity();

	void Invert();

	void Translate ( Real x, Real y, Real z);

	void Rotate(Real Yaw, Real Pitch, Real Roll);
	void Scale( Real x, Real y, Real z );

	void RotateYaw   ( Real Rad ) ;
	void RotatePitch ( Real Rad ) ;
	void RotateRoll  ( Real Rad ) ;
	
};*/

struct Vector2D {
	Real x, y;

	Vector2D() { x=0; y=0; }
	Vector2D(Real _x, Real _y) { x = _x; y = _y; }
};

struct Vector3D {

	Real x, y, z;

	Vector3D() { x = 0; y = 0; z = 0; }
	Vector3D( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; }
	Vector3D( float* v ) { x = v[0]; y = v[1]; z = v[2]; }
	Vector3D( double* v ) { x = (float)v[0]; y = (float)v[1]; z = (float)v[2]; }

//	void Rotatex ( Real Rad );
//	void Rotatey ( Real Rad );
//	void Rotatez ( Real Rad );

//	float CutLength( float amount );
//	void  SetLength( float l );


	void   operator = ( const Vector3D& m ) { x=m.x; y=m.y; z=m.z;  }

	Real Dot ( const Vector3D &V ) const;
	Vector3D Cross ( const Vector3D &V ) const;

	Real Mag () const;
	Real MagSq() const;
	Real Length() const { return Mag(); }
	Real LengthSq() const { return MagSq(); }

//	Real Angle ( Vector3D &V );
//	Real AngleNorm ( Vector3D &V );

//	Vector3D Reflect( Vector3D& Normal );


//	void Transform( Matrix& matrix );


	Vector3D Normalized();

	float operator [] ( int index );
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


struct Plane
{
	Real a, b, c, d;

	Plane();
	Plane( float a, float b, float c, float d );
	Plane( const Vector3D* triangle);

	Real DistanceToPoint(const Vector3D& Point){return ((Point.x*a)+(Point.y*b)+(Point.z*c)+d);}
	bool IntersectLine(const Vector3D& p1, const Vector3D& p2, Vector3D* result);

	static Vector3D TriangleNormal( const Vector3D* vertices);
	static bool IntersectLineWithTriangle( const Vector3D* vertices, const Vector3D& p1, const Vector3D& p2, Vector3D* result);
};



/*struct Quaternion
{
	Real w, x, y, z;

	Quaternion() ;
	

	void Invert();

	Quaternion Slerp( Quaternion& Q, Real t) ;

	void Rotate(Real Yaw, Real Pitch, Real Roll) ;


	Quaternion operator * ( Quaternion Q );
//	void operator = ( Matrix& Mat);

};
*/



struct BoundingVolume 
{
//virtual CalcBoundingVolume()
};


struct BoundingBox : public BoundingVolume
{
	Vector3D BoxCoords[8];
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
	AABB( const Vector3D& a, const Vector3D& b )
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
