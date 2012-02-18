// I N C L U D E S //////////////////////////////
#include "..\Physics\Construction.h"

// C L A S S E S ////////////////////////////////

class TankConstruct : public Construction
{
protected:
	MeshHandle ChassisMesh;
	MeshHandle TurretMesh;
	MeshHandle WheelMesh;
	MeshHandle DriveWheelMesh;
	MeshHandle GunMesh;
	MeshManager* pMM;

	LPD3DXMESH d3dxWheelMesh;
	LPD3DXMESH d3dxDriveWheelMesh;

	float ChassisWidth, ChassisHeight, ChassisLength;
	float ChassisX, ChassisY, ChassisZ;
	float ChassisMass;
	float TurretZOffset, TurretYOffset;
	float GunYOffset, GunZOffset,GunXOffset;
	Real  WheelXOffset, WheelYOffset;

	int nWheels;
	Real WheelRadius;
	Real DriveWheelRadius;


	Real CurrentSpeed; 
	Real MaxSpeed;
	Real Throttle;
	Real TurnForce;
	Real TurretYawVel;
	Real TurretPitchVel;
	bool LeftClutch;
	bool RightClutch;
	
	//Center of gravity
	float CenterX, CenterY, CenterZ;

public:
	TankConstruct();

	std::string GetTypeString();
	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler=NULL  );

	void OnTurn( Real Force, Real Angle );
	void OnAccelerate( Real pedalForce, Real SideForce=0, Real UpForce=0 );

	bool CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM );

	void Update( Real FrameTime, WorldManager* SM );

	void GetMatrix( Matrix* M );

	void Draw(Screen3D& Screen);


	//custom functions
	Real GetCurrentSpeed() { return CurrentSpeed; }
	Real GetTurretYawSpeed() { return TurretYawVel; }
	int  GetTurretIndex() { return nObjects-2; }

	float GetGunYOffset() { return GunYOffset; }
	float GetGunZOffset() { return GunZOffset; }


};