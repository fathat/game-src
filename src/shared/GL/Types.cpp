#include <fstream.h>
#include "types.h"
#include <stdio.h> 
#include <stdarg.h>




std::string GetRelativeDirectory( char* WorkingDirectory, char* In )
{
	//Find out where the dir is relative to this
	int CommonDirLength=0;
	int si=0;

	char* file = In;
	char* commondir = WorkingDirectory;
	bool NotCommon=false;
	
	//Keep looping until there is a difference in the path for the working directory
	//and the file's directory
	while( !NotCommon )
	{

		//Check to see if there is a difference between strings
		if(!NotCommon)
		{
			if(tolower(file[si]) != tolower(commondir[si]))
			{
				NotCommon=true;
				break;
			}
		}

		//Check to see if we've reached the path symbol, if we
		//have update the path index variables.
		if(file[si] == '\\' || file[si] == '/')
		{
			if(!NotCommon  )
			{
				CommonDirLength = si;
			}
	
		}

		si++;
	}

	//Now that we have all the stuff, construct a relative directory
	std::string ActualFilename;							
	ActualFilename = "..";
	ActualFilename += &file[CommonDirLength];

	return ActualFilename;

}

void OutputMessage(char* text, ...)
{

	static char OutMessage[2048];
	ZeroMemory(OutMessage, 2048);

	va_list argumentlist;

	va_start(argumentlist, text);
	vsprintf(OutMessage, text, argumentlist);

	ofstream file;

	file.open("mage3D debug.txt", ios::app, filebuf::openprot);
	file.write(OutMessage, strlen(text));
	file.write("\n", 1);
	file.close();

}

void OutputDXError(HRESULT Error, char* text)
{
	ofstream file;

	char ErrorString[64];
	D3DXGetErrorStringA(Error, ErrorString, 64);

	file.open("debug.txt", ios::app, filebuf::openprot);
	file.write(text, strlen(text));
	file.write(ErrorString, strlen(ErrorString));
	file.write("\n", 1);
	file.close();


}

Matrix Matrix::operator + ( const Matrix& m )
{
	Matrix Result;
	Result.Mat = Mat + m.Mat;

	return Result;
}

Matrix Matrix::operator - ( const Matrix& m )
{
	Matrix Result;
	Result.Mat = Mat - m.Mat;

	return Result;
}

Matrix Matrix::operator * ( Matrix& m )
{
	Matrix Result;
	D3DXMatrixMultiply(&Result.Mat, &Mat, &m.Mat);
	
	return Result;
}



//vector stuff starts here//
void Vector3D::Rotatex ( Real Rad ) {
   Real oj = y, ok = z;

   y = ( Real ) ( oj * cos ( Rad ) - ok * sin ( Rad ) );
   z = ( Real ) ( oj * sin ( Rad ) + ok * cos ( Rad ) );
}

void Vector3D::Rotatey ( Real Rad ) {
   Real oi = x, ok = z;

   x = ( Real ) ( oi * cos ( Rad ) - ok * sin ( Rad ) );
   z = ( Real ) ( oi * sin ( Rad ) + ok * cos ( Rad ) );
}

void Vector3D::Rotatez ( Real Rad ) {
   Real oi = x, oj = y;

   x = ( Real ) ( oi * cos ( Rad ) - oj * sin ( Rad ) );
   y = ( Real ) ( oi * sin ( Rad ) + oj * cos ( Rad ) );
}

Real Vector3D::Dot ( Vector3D &V ) {
   return ( x + V.x + y * V.y + z * V.z );
}

Vector3D Vector3D::Cross ( Vector3D &V ) {
   Vector3D Result;

   Result.x = y * V.z - z * V.y;
   Result.y = z * V.x - x * V.z;
   Result.z = x * V.y - y * V.x;

   return Result;
}

Real Vector3D::Mag () {
   return ( Real ) sqrt ( x * x + y * y + z * z );
}

Real Vector3D::Angle ( Vector3D &V ) {
   Real ProdMag = Mag () * V.Mag ();
   return ( Real ) atan ( Dot ( V ) / ProdMag );
}

Real Vector3D::AngleNorm ( Vector3D &V ) {
   return ( Real ) acos ( Dot ( V ) );
}

Vector3D::operator Real () {
   return this->Mag ();
}

Vector3D::operator Point3D () {
   Point3D P;

   P.x = x; P.y = y; P.z = z;

   return P;
}

Vector3D &Vector3D::Normalize () {
   Real Dist = Mag ();

   x /= Dist;
   y /= Dist;
   z /= Dist;

   return *this;
}

Vector3D Vector3D::operator - ( Vector3D &V ) {
   Vector3D Result;

   Result.x = x - V.x;
   Result.y = y - V.y;
   Result.z = z - V.z;

   return Result;
}

Vector3D Vector3D::operator + ( Vector3D &V ) {
   Vector3D Result;

   Result.x = x + V.x;
   Result.y = y + V.y;
   Result.z = z + V.z;

   return Result;
}

Vector3D Vector3D::operator * ( Real S ) {
   Vector3D Result;

   Result.x = x * S;
   Result.y = y * S;
   Result.z = z * S;

   return Result;
}

void Point3D::Rotatex ( Real Rad ) {
   Real oy = y, oz = z;

   y = ( Real ) ( oy * cos ( Rad ) - oz * sin ( Rad ) );
   z = ( Real ) ( oy * sin ( Rad ) + oz * cos ( Rad ) );
}

void Point3D::Rotatey ( Real Rad ) {
   Real ox = x, oz = z;

   x = ( Real ) ( ox * cos ( Rad ) - oz * sin ( Rad ) );
   z = ( Real ) ( ox * sin ( Rad ) + oz * cos ( Rad ) );
}

void Point3D::Rotatez ( Real Rad ) {
   Real ox = x, oy = y;

   x = ( Real ) ( ox * cos ( -Rad ) - oy * sin ( -Rad ) );
   y = ( Real ) ( ox * sin ( -Rad ) + oy * cos ( -Rad ) );
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

   return ( Real ) sqrt ( Dx * Dx + Dy * Dy + Dz * Dz );
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

MeshMaterial::MeshMaterial () { 
	Image = NULL; 

	AmbientRGBA.r = 0.1f;
	AmbientRGBA.g =	0.1f;
	AmbientRGBA.b =	0.1f;
	AmbientRGBA.a =	1;

	DiffuseRGBA.r = 0.1f;
	DiffuseRGBA.g =	0.1f;
	DiffuseRGBA.b =	0.1f;
	DiffuseRGBA.a =	1;

	SpecularRGBA.r = 0.1f;
	SpecularRGBA.g = 0.1f;
	SpecularRGBA.b = 0.1f;
	SpecularRGBA.a = 1;

	Shininess    = 0;
	Transparency = 0;
	Image = 0;
}
MeshMaterial::~MeshMaterial () { Image = NULL; }

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


void Camera::Follow( Position& Location, Real Distance, Real PitchOveride )
{
	// Set up our view matrix. (Using quaternions!)
	x = Location.x + Distance * (Real)cos(Location.Yaw + RADIAN(90)) ;
	y = Location.y + Distance;
	z = Location.z - Distance * (Real)sin(Location.Yaw + RADIAN(90)) ;

	

	Yaw = Location.Yaw;

	if(PitchOveride != -1)
		Pitch = PitchOveride;
	else
		Pitch = Location.Pitch;

	Roll  = 0;

}

Matrix Camera::GetLookAtMatrix( Real Distance )
{
	Matrix Result;
	D3DXMATRIX matR, matPosition, matView, matT, matMoveBack;
	D3DXQUATERNION qR;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matPosition);


	D3DXMatrixTranslation(&matMoveBack, 0, 0, -Distance);
	        
    D3DXMatrixTranslation(&matT, x, y, z);
    D3DXMatrixMultiply(&matPosition, &matT, &matPosition);

    D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    D3DXMatrixRotationQuaternion(&matR, &qR);


	D3DXMatrixMultiply(&matR, &matMoveBack, &matR);
    D3DXMatrixMultiply(&matPosition, &matR, &matPosition);
	D3DXMatrixInverse(&matView, NULL, &matPosition);

	Result.Mat = matView;


	
	return Result;
	

}


Matrix Camera::GetViewMatrix( )
{

	D3DXMATRIX matR, matPosition, matView, matT;

	D3DXQUATERNION qR;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matPosition);
        
    D3DXMatrixTranslation(&matT, x, y, z);
    D3DXMatrixMultiply(&matPosition, &matT, &matPosition);

    D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    D3DXMatrixRotationQuaternion(&matR, &qR);

    D3DXMatrixMultiply(&matPosition, &matR, &matPosition);
    D3DXMatrixInverse(&matView, NULL, &matPosition);
	
	Matrix M;
	M.Mat = matView;
	

	return M;
}