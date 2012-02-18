#include "types.h"
#include "screen.h"
#include <fstream>
#include <dxerr.h>

using namespace std;


float	(*m_atan)(float);
float	(*m_atan2)(float,float);
float	(*m_acos)(float);
float	(*m_asin)(float);
float  (*m_log)(float);
float  (*m_log10)(float);
float  (*m_pow)(float, float);
float	(*m_exp)(float);
float  (*m_sqrt)(float);
float 	(*m_fabs)(float);
float  (*m_ceil)(float);
float 	(*m_floor)(float);
float 	(*m_frexp)(float, int *);
float	(*m_ldexp)(float, int);
float  (*m_modf)(float, float *);
float	(*m_fmod)(float, float);
void 	(*m_sincos)(float, float *);
float	(*m_sin)(float);
float	(*m_cos)(float);
float  (*m_tan)(float);



float	s_atan(float f) { return (float)atan(f); }
float	s_atan2(float f,float f2 ) { return (float)atan2(f, f2); }
float	s_acos(float f) { return (float)acos(f); }
float	s_asin(float f) { return (float)asin(f); }
float	s_log(float f) { return (float)log(f); }
float	s_log10(float f) { return (float)log10(f); }
float	s_pow(float f, float f2) { return (float)pow(f, f2); }
float	s_exp(float f) { return (float)exp(f); }
float	s_sqrt(float f) { return (float)sqrt(f); }
float 	s_fabs(float f) { return (float)fabs(f); }
float	s_ceil(float f) { return (float)ceil(f); }
float 	s_floor(float f) { return (float)floor(f); }
float 	s_frexp(float f, int *f2) { return (float)frexp(f, f2); }
float	s_ldexp(float f, int f2) { return (float)ldexp(f, f2); }
float	s_modf(float f, float *f2) { return (float)modf(f, (double*)f2); }
float	s_fmod(float f, float f2) { return (float)fmod(f, f2); }
void 	s_sincos(float f, float *f2);
float	s_sin(float f) { return (float)sin(f); }
float	s_cos(float f) { return (float)cos(f); } 
float	s_tan(float f) { return (float)tan(f); }




void OutputMessage(char* text, ...)
{

	static char OutMessage[2048];
	ZeroMemory(OutMessage, 2048);

	va_list argumentlist;

	va_start(argumentlist, text);
	vsprintf(OutMessage, text, argumentlist);
	va_end(argumentlist);

	FILE* file = fopen( "debug.txt", "at");
	fprintf(file, OutMessage );
	fprintf(file, "\n" );
	fclose(file );

/*	ofstream file;

	file.open("debug.txt", ios::app, filebuf::openprot);
	file.write(OutMessage, strlen(text));
	file.write("\n", 1);
	file.close();*/

}

void OutputDXError(HRESULT Error, char* text)
{
	ofstream file;

	//char ErrorString[64];
	const char* ErrorString =  DXGetErrorString ( Error );
	//D3DXGetErrorString(Error, ErrorString, 64);

	file.open("debug.txt", ios::app);
	file.write(text, strlen(text));
	file.write(ErrorString, strlen(ErrorString));
	file.write("\n", 1);
	file.close();


	OutputDebugString( text );
	OutputDebugString( ErrorString) ;
	OutputDebugString( "\n" );
	


}

Real CutValue( Real Value, Real amount )
{

	if( Value > 0 )
	{
		Value -= amount;
		if(Value < 0)
			Value = 0;
	}
	if( Value < 0 )
	{
		Value += amount;
		if(Value > 0)
			Value = 0;
	}

	return Value;

}



Matrix Matrix::operator + ( const Matrix& m ) const
{
	Matrix Result;
	Result.Mat = Mat + m.Mat;

	return Result;
}

Matrix Matrix::operator - ( const Matrix& m ) const
{
	Matrix Result;
	Result.Mat = Mat - m.Mat;

	return Result;
}

Matrix Matrix::operator * ( Matrix& m ) const
{
	Matrix Result;
	D3DXMatrixMultiply(&Result.Mat, &Mat, &m.Mat);
	
	return Result;
}



//vector stuff starts here//
void Vector3D::Rotatex ( Real Rad ) {
   Real oy = y, oz = z;

   y = ( Real ) ( oy * (*m_cos) ( Rad ) - oz * (*m_sin) ( Rad ) );
   z = ( Real ) ( oz * (*m_cos) ( Rad ) + oy * (*m_sin) ( Rad ) );
}

void Vector3D::Rotatey ( Real Rad ) {
   Real ox = x, oz = z;

   z = ( Real ) ( oz * (*m_cos) ( Rad ) + ox * (*m_sin) ( Rad ) );
   x = ( Real ) ( ox * (*m_cos) ( Rad ) - oz * (*m_sin) ( Rad ) );
}

void Vector3D::Rotatez ( Real Rad ) {
   Real oi = x, oj = y;

   x = ( Real ) ( oi * (*m_cos) ( Rad ) - oj * (*m_sin) ( Rad ) );
   y = ( Real ) ( oi * (*m_sin) ( Rad ) + oj * (*m_cos) ( Rad ) );
}

Real Vector3D::Dot ( Vector3D &V ) {

   return ( x * V.x + y * V.y + z * V.z );
}


Vector3D Vector3D::Reflect( Vector3D& Normal )
{
	Vector3D NormalizedVector = *this;
	NormalizedVector.Normalize();
	Vector3D doubledNormal = Normal * 2.0f;
	float dot = NormalizedVector.Dot(Normal) * this->Mag();
	Vector3D temp = doubledNormal * dot;
	
	return NormalizedVector -  temp;
}

Vector3D Vector3D::Cross ( Vector3D &V ) {
   Vector3D Result;

   Result.x = y * V.z - z * V.y;
   Result.y = z * V.x - x * V.z;
   Result.z = x * V.y - y * V.x;

   return Result;
}

Real Vector3D::Mag () const {
   return ( Real ) (*m_sqrt) ( x * x + y * y + z * z );
}

Real Vector3D::MagSq() const {
	return ( x * x + y * y + z * z );
}

Real Vector3D::Angle ( Vector3D &V ) {
   Real ProdMag = Mag () * V.Mag ();
   return ( Real ) atan ( Dot ( V ) / ProdMag );
}

Real Vector3D::AngleNorm ( Vector3D &V ) {
   return ( Real ) (*m_acos) ( Dot ( V ) );
}

Vector3D::operator Real () {
   return this->Mag ();
}

Vector3D::operator Point3D () {
   Point3D P;

   P.x = x; P.y = y; P.z = z;

   return P;
}

Vector3D::operator D3DXVECTOR3 () {
   static D3DXVECTOR3 V;

   V.x = x; V.y = y; V.z = z;

   return V;
}

void Vector3D::Transform( Matrix& matrix )
{
	D3DXVECTOR4 output;
	D3DXVec3Transform( &output, (const D3DXVECTOR3*)this, &matrix.Mat );

	this->x = output.x;
	this->y = output.y;
	this->z = output.z;
}

void  Vector3D::SetLength( float l )
{
	Normalize();
	x *= l;
	y *= l;
	z *= l;
}

float Vector3D::CutLength( float amount )
{
    float m = Mag();
	float l = CutValue( m, fabs(amount) );
	SetLength(l);

	return l;
	
}

Vector3D &Vector3D::Normalize () {
   Real Dist = Mag ();

   if(Dist <= 0 )
	   return *this;

   x /= Dist;
   y /= Dist;
   z /= Dist;

   return *this;
}

float& Vector3D::operator [] ( int index )
{
	if ( index == 0 )
		return x;
	if ( index == 1 )
		return y;
	if( index == 2 )
		return z;
	return x;
}

/*void  operator [] ( int index, float value )
{
	if( index == 0 )
		x = value;
	if( index == 1 )
		y = value;
	if( index == 2 )
		z = value;
	

}*/

Vector3D Vector3D::operator - ( const Vector3D &V ) const {
   Vector3D Result;

   Result.x = x - V.x;
   Result.y = y - V.y;
   Result.z = z - V.z;

   return Result;
}

Vector3D Vector3D::operator + ( const Vector3D &V ) const {
   Vector3D Result;

   Result.x = x + V.x;
   Result.y = y + V.y;
   Result.z = z + V.z;

   return Result;
}

Vector3D Vector3D::operator * ( const Vector3D &V ) const
{
	Vector3D Result;
	Result.x = x * V.x;
	Result.y = y * V.y;
	Result.z = z * V.z;
	return Result;
}

Vector3D Vector3D::operator / ( const Vector3D &V ) const
{
	Vector3D Result;
	Result.x = x / V.x;
	Result.y = y / V.y;
	Result.z = z / V.z;
	return Result;
}


void Vector3D::operator += ( const Vector3D &V )  {
	x += V.x;
	y += V.y;
	z += V.z;
}
void Vector3D:: operator -= ( const Vector3D &V )  {
	x -= V.x;
	y -= V.y;
	z -= V.z;
}

Vector3D Vector3D::operator * ( const Real S ) const {
   static Vector3D Result;

   Result.x = x * S;
   Result.y = y * S;
   Result.z = z * S;

   return Result;
}

Vector3D Vector3D::operator / ( const Real S ) const {
   static Vector3D Result;

   Result.x = x / S;
   Result.y = y / S;
   Result.z = z / S;

   return Result;
}

void  Vector3D::operator *= ( const Real S )
{
	x *= S;
	y *= S;
	z *= S;
}


void  Vector3D::operator /= ( const Real S )
{
	x /= S;
	y /= S;
	z /= S;
}

void  Vector3D::operator += ( const Real S )
{
	x += S;
	y += S;
	z += S;
}

void  Vector3D::operator -= ( const Real S )
{
	x -= S;
	y -= S;
	z -= S;
}

void Point3D::Rotatex ( Real Rad ) {
   Real oy = y, oz = z;

   y = ( Real ) ( oy * (*m_cos) ( Rad ) - oz * (*m_sin) ( Rad ) );
   z = ( Real ) ( oz * (*m_cos) ( Rad ) + oy * (*m_sin) ( Rad ) );
}

void Point3D::Rotatey ( Real Rad ) {
   Real ox = x, oz = z;

   z = ( Real ) ( oz * (*m_cos) ( Rad ) + ox * (*m_sin) ( Rad ) );
   x = ( Real ) ( ox * (*m_cos) ( Rad ) - oz * (*m_sin) ( Rad ) );
   
}

void Point3D::Rotatez ( Real Rad ) {
   Real ox = x, oy = y;

   x = ( Real ) ( ox * (*m_cos) ( -Rad ) - oy * (*m_sin) ( -Rad ) );
   y = ( Real ) ( ox * (*m_sin) ( -Rad ) + oy * (*m_cos) ( -Rad ) );
}

Point3D::operator Vector3D () {
   Vector3D V;

   V.x = x; V.y = y; V.z = z;

   return V;
}

Point3D Point3D::operator - ( Point3D &P ) {
   Point3D Result;

   Result.x = x - P.x;
   Result.y = y - P.y;
   Result.z = z - P.z;

   return Result;
}

Point3D Point3D::operator + ( Point3D &P ) {
   Point3D Result;

   Result.x = x + P.x;
   Result.y = y + P.y;
   Result.z = z + P.z;

   return Result;
}

Point3D Point3D::operator * ( Real S ) {
   Point3D Result;

   Result.x = x * S;
   Result.y = y * S;
   Result.z = z * S;

   return Result;
}

Real Point3D::DistanceTo ( Point3D &P ) {
   Real Dx = P.x - x,
        Dy = P.y - y,
        Dz = P.z - z;

   return ( Real ) (*m_sqrt) ( Dx * Dx + Dy * Dy + Dz * Dz );
}





Quaternion Quaternion::operator * ( Quaternion Q )
{
	Quaternion Result;
	D3DXQuaternionMultiply( &Result.Quat, &Quat, &Q.Quat );
	return Result;
	
}


void Quaternion::operator = ( Matrix& Mat)
{
	D3DXQuaternionRotationMatrix(&Quat, &Mat.Mat);
}


Position::operator Vector3D () {
   Vector3D v;

   v.x = x; v.y = y; v.z = z;

   return v;
}


Matrix Position::GetMatrix()
{
	D3DXMATRIX matR, matPosition, matT;
	D3DXQUATERNION qR;
	
	D3DXMatrixIdentity(&matPosition);
    D3DXMatrixTranslation(&matT, x, y, z);
    
    D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    D3DXMatrixRotationQuaternion(&matR, &qR);

    D3DXMatrixMultiply(&matPosition, &matR, &matT);
    	
	Matrix M;
	M.Mat = matPosition;

	return M;

}



Matrix Position::GetRotationMatrix()
{
	D3DXMATRIX matR, matR2;
	D3DXQUATERNION qR;  
    D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    
	D3DXMatrixRotationQuaternion(&matR, &qR);

        	
	Matrix M;
	M.Mat = matR;
	return M;
}


SoundSource Position::AsSoundSource()
{
	Vector3D v(0,0,-1);
	Matrix rotation = GetRotationMatrix();
	v.Transform( rotation );

	return SoundSource( x, y, z, 0, 0, 0,v.x, v.y, v.z );
}