#pragma once

class Construction;
class DynamicsObject;


class DynamicsSurfaceDesc
{
public:
	float ContactSlip1;
	float ContactSlip2;
	float Bounce;
	float BounceVelocity;
	float SoftERP;
	float SoftCFM;
	float mu, mu2;


	DynamicsObject* Owner; //not required to have an owner
	Construction*   ParentConstruction;
public:
	DynamicsSurfaceDesc();
	~DynamicsSurfaceDesc();

	void Combine (DynamicsSurfaceDesc* d1, DynamicsSurfaceDesc* d2 );
};
