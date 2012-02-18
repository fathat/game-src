#include "DynamicsObject.h"
#include "Solver.h"

DynamicsObject::DynamicsObject() 
{ 
	Owner = NULL;
	isAvatar = false;
	Setup = false; 
	HasGeom = false;
}
DynamicsObject::~DynamicsObject() 
{ 
	Destroy(); 
}


void DynamicsObject::CreateBody(DynamicsSolver* solver)
{
	Body = dBodyCreate(solver->GetWorldID());
	SetData();
}

void DynamicsObject::SetData()
{
	dBodySetData( Body, this );
}

void DynamicsObject::OnCollide(ICollisionHandler* objectHit )
{
	this->Owner->Activate ();
	this->Owner->OnCollide( objectHit );

}

void DynamicsObject::OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount )
{
	Owner->OnDamage( source, collisionPoint, damageAmount );
}


void DynamicsObject::Destroy()
{
	if(Setup)
	{

		dBodyDestroy( Body );

		if(HasGeom)
			dGeomDestroy( Geom );
	}
	Setup = false;
}
