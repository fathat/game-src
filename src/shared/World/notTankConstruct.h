// I N C L U D E S //////////////////////////////
#include "Construction.h"

// C L A S S E S ////////////////////////////////

class TankConstruct : public Construction
{
protected:
	MeshHandle ChassisMesh;
	MeshHandle TurretMesh;
	MeshHandle WheelMesh;
	MeshHandle DriveWheelMesh;
	MeshManager* pMM;

	LPD3DXMESH d3dxWheelMesh;
	LPD3DXMESH d3dxDriveWheelMesh;

	float ChassisWidth, ChassisHeight, ChassisLength;
	float ChassisX, ChassisY, ChassisZ;
	float ChassisMass;
	Real  WheelXOffset, WheelYOffset;

	int nWheels;
	Real WheelRadius;
	Real DriveWheelRadius;


	Real CurrentSpeed; 
	Real MaxSpeed;
	Real Throttle;
	Real TurnForce;
	bool LeftClutch;
	bool RightClutch;
	
	//Center of gravity
	float CenterX, CenterY, CenterZ;

public:
	Vehicle4Wheeled();

	void GetTypeString(char* typebuffer, int maxsize);

	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location );

	void Turn( Real Force, Real Angle );
	void Accelerate( Real pedalForce, Real SideForce=0, Real UpForce=0 );

	void Update( Real FrameTime, WorldManager* SM );

	void GetMatrix( Matrix* M );

	void Draw(Screen3D& Screen);


};