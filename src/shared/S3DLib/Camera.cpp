#include "camera.h"
	
#include "..\defs\defs.h"


Camera::Camera() 
{
	ZeroMemory(this, sizeof( Camera ) );
	FOV = 75;
}


//Gets the camera matrix (at given distance from target).
//Note that distance is ignored when in first person mode. 
Matrix Camera::GetLookAtMatrix( Real Distance )
{
	if( Pos == NULL )
		return Matrix::MakeIdentity();

	Matrix Result;
	D3DXMATRIX matR, matPosition, matView, matT, matMoveBack;
	D3DXQUATERNION qR;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matPosition);

	if(CS == CS_FIRST_PERSON)
		Distance=0;
	D3DXMatrixTranslation(&matMoveBack, 0, 0, -Distance);

	if( CS == CS_FOLLOW_MATRIX )
	{
		D3DXMATRIX tempMat;
		
		D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
	    D3DXMatrixRotationQuaternion(&matR, &qR);

		D3DXMatrixMultiply(&matR, &matMoveBack, &matR);
		D3DXMatrixMultiply (&tempMat, &matR,&FollowMat->Mat );
		D3DXMatrixInverse(&matView, NULL, &tempMat );
		Result.Mat = matView;
		return Result;
	}
	

	if(CS == CS_FIRST_PERSON)
		D3DXMatrixTranslation(&matT, Pos->x + OffsetX, Pos->y + OffsetY + HeightDisplacement, Pos->z + OffsetZ);
	else
		D3DXMatrixTranslation(&matT, Pos->x + OffsetX, Pos->y + OffsetY, Pos->z + OffsetZ);

    D3DXMatrixMultiply(&matPosition, &matT, &matPosition);


	D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    D3DXMatrixRotationQuaternion(&matR, &qR);


	D3DXMatrixMultiply(&matR, &matMoveBack, &matR);
    D3DXMatrixMultiply(&matPosition, &matR, &matPosition);
	D3DXMatrixInverse(&matView, NULL, &matPosition);

	Result.Mat = matView;
	return Result;
}




Matrix Camera::GetRotationMatrix()
{
	D3DXQUATERNION qR;
	D3DXMATRIX matView, matR, matR2;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matR);

	D3DXQuaternionRotationYawPitchRoll(&qR, Yaw, Pitch, Roll);
    D3DXMatrixRotationQuaternion(&matR, &qR);

	if(CS == CS_FOLLOW_MATRIX )
	{
		D3DXMatrixRotationQuaternion(&matR2, &qR);
		D3DXMatrixMultiply ( &matR,  &matR2, &FollowMatSky->Mat );
	}
	else
	{
		D3DXMatrixRotationQuaternion(&matR, &qR);
	}

	D3DXMatrixInverse(&matView, NULL, &matR);

	Matrix M;
	M.Mat = matView;

	return M;

}


//Updates camera
void Camera::Update( Real FrameTime )
{
	Yaw   += YawDrift * FrameTime;
	Pitch += PitchDrift * FrameTime;
	Roll  += RollDrift * FrameTime;

	Zoom += ZoomDrift * FrameTime;

	OffsetX += XDrift*FrameTime;
	OffsetY += YDrift*FrameTime;
	OffsetZ += ZDrift*FrameTime;

	FOV += FOVDrift*FrameTime;
}


