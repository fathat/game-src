// I N C L U D E S //////////////////////////////
#include "..\Physics\Construction.h"

// C L A S S E S ////////////////////////////////
struct Wheel
{
	float Radius;
	Vector3D Location;
};

class Vehicle4Wheeled : public Construction
{
protected:
	bool AllWheelSteering;
	MeshHandle ChassisMesh;
	MeshHandle WheelMesh;
	MeshManager* pMM;

	Wheel WheelList[4];
	float ChassisWidth, ChassisHeight, ChassisLength;
	float ChassisX, ChassisY, ChassisZ;
	float ChassisMass;
	

	Real  FrontWheelZOffset;
	Real  BackWheelZOffset;
	Real  WheelXOffset;
	Real  WheelYOffset;


	float SteeringAngle;
	float SteeringForce;
	
	float MaxSpeed;
    
	Real PedalForce; //between -100 and 100, 100 being gas to the floor
					  //-100 being brake mashed
	Real CurrentSpeed; //HMMMMMM
	Real RPM;
	int  Gear;		   
	bool ClutchDepressed; 
	bool HandBrake;

	LPD3DXMESH d3dxWheelMesh;

	

	//Center of gravity
	float CenterX, CenterY, CenterZ;

public:
	Vehicle4Wheeled();

	std::string GetTypeString();

	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler=NULL  );

	void OnTurn( Real Force, Real Angle );
	void OnAccelerate( Real pedalForce, Real SideForce=0, Real UpForce=0 );

	bool CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM );

	void Update( Real FrameTime, WorldManager* SM );

	void GetMatrix( Matrix* M );

	void Draw(Screen3D& Screen);

	//custom functions (not derived from anything)
	Real GetRPM() { return RPM; }
	Real GetCurrentSpeed() { return CurrentSpeed; }
	void SetClutch( bool c ) { ClutchDepressed = c; }
	bool GetClutchDepressed() { return ClutchDepressed; }
	void SetGear( int g );
	int  GetGear() { return Gear; }
	Real GetGearTorque( int g, Real rpm );
	Real GetGearTopSpeed(int g );
	Real GetRPMForGear( int g, Real torque );

	void SetHandBrake( bool h ) { HandBrake = h; }
	bool GetHandBrake() { return HandBrake; }

};