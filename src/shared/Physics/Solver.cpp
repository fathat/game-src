#include <stdlib.h>

#include "Solver.h"
#include "Construction.h"
#include "Structure.h"
#include "..\world\avatarconstruction.h"
#include "..\cvar\CVar.h"


#include "..\defs\defs.h"

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.


int compareContacts( const void *arg1, const void *arg2 )
{
	dContact* c1 = *((dContact**)arg1);
	dContact* c2 = *((dContact**)arg2);

	float v = fabs(c2->geom.depth)  - fabs(c1->geom.depth);
	if( v > 0 )
		return 1;
	if ( v< 0 )
		return -1;

	return 0;

}


	
void fixFrictionVector( dBodyID b1, dBodyID b2, dContact& contact )
{
	dBodyGetPointVel(b1, contact.geom.pos[0], contact.geom.pos[1], contact.geom.pos[2], contact.fdir1);
	dVector3 fdir1_b2;
	if (b2)
	{
		dBodyGetPointVel(b2, contact.geom.pos[0], contact.geom.pos[1],	contact.geom.pos[2], fdir1_b2);
		contact.fdir1[0] -= fdir1_b2[0];
		contact.fdir1[1] -= fdir1_b2[1];
		contact.fdir1[2] -= fdir1_b2[2];
	}
	// at this point, contact[i].fdir1 is the relative tangent velocity of the two bodies.
	dCROSS(contact.fdir1, =, contact.fdir1, contact.geom.normal);
	// now, contact[i].fdir1 is perpendicular to both the normal and
	// the relative tangent velocity.
	double length = sqrt(contact.fdir1[0] * contact.fdir1[0]
	       			   + contact.fdir1[1] * contact.fdir1[1]
				       + contact.fdir1[2] * contact.fdir1[2]);
	if (length > 1e-12)
	{
	       // we only use our calculated direction if it has enough precision
	       contact.fdir1[0] /= length;
	       contact.fdir1[1] /= length;
	       contact.fdir1[2] /= length;
	       dNormalize3(contact.fdir1);
	       contact.surface.mode |= dContactFDir1;
	}

}


static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
	//make sure both of the geom's are enabled (otherwise this is
	//a waste of our time
	if( !dGeomIsEnabled( o1 ) || !dGeomIsEnabled (o2 ))
	{
		return;
	}


	//collide spaces if neccesary
	if (dGeomIsSpace (o1) || dGeomIsSpace (o2)) 
	{
		// collide the space(s?)
		dSpaceCollide2 (o1,o2,data,&nearCallback);

		// collide all geoms/spaces within the spaces
		//if (dGeomIsSpace (o1)) dSpaceCollide ((dSpaceID)o1,data,&nearCallback);
		//if (dGeomIsSpace (o2)) dSpaceCollide ((dSpaceID)o2,data,&nearCallback);
    }

	//otherwise no spaces, just collide geoms
	else 
	{

		//make sure one of the geoms has a body
		dBodyID body1 = dGeomGetBody( o1 );
		dBodyID body2 = dGeomGetBody( o2 );
		//if( body1 == 0 && body2 == 0)
			//return;
		
		//make sure that the bodies are enabled
		
		

		//declarations
		DynamicsObject* dynobj=NULL, *dynobj2=NULL;
		int n;

		DynamicsSolver* solver = (DynamicsSolver*)data; 

		
		

		//Get 64 contacts
		const int N = 64;
		dContact contact[N];
		n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));

		//Sort all the contacts
		dContact* sortedContacts[N];
		for( int i=0; i<n; i++ ) sortedContacts[i] = &contact[i];
		qsort( sortedContacts, n,sizeof( dContact*), compareContacts );

		//determine how many contacts to actually care about
		//if( n > 8 )
		//	n = 8;
		
		
		if (n > 0) 
		{
			//figure out surface desc stuff
			DynamicsSurfaceDesc* sd1, *sd2;
			DynamicsSurfaceDesc finaldesc;
			sd1 = (DynamicsSurfaceDesc*)dGeomGetData(contact[0].geom.g1);
			sd2 = (DynamicsSurfaceDesc*)dGeomGetData(contact[0].geom.g2);
 			finaldesc.Combine( sd1, sd2);
		

			//Get the bodies involved in the collision
			dBodyID b1, b2;
			b1 = dGeomGetBody(contact[0].geom.g1);
			b2 = dGeomGetBody(contact[0].geom.g2);

			//Inform objects of the collision that occured.
			if(b1) 	dynobj = (DynamicsObject*)dBodyGetData(b1);
			if(b2)	dynobj2 = (DynamicsObject*)dBodyGetData(b2);
			if( dynobj ) dynobj->OnCollide ( dynobj2 );
			if( dynobj2) dynobj2->OnCollide( dynobj);


			//Generate contact joints
			for (int i=0; i<n; i++) 
			{
				contact[i].surface.mode =  dContactSoftERP | dContactSoftCFM | dContactApprox1;// | dContactBounce;
				//contact[i].surface.mu = 0.5f;
				contact[i].surface.mu = finaldesc.mu;
				
				
				if(contact[i].geom.normal[1] < .8f )
				{
					contact[i].surface.mu = 0.1f;
				}

				contact[i].surface.slip1 = finaldesc.ContactSlip1 ;
				contact[i].surface.slip2 = finaldesc.ContactSlip2 ;
				contact[i].surface.soft_erp = finaldesc.SoftERP ;
				contact[i].surface.soft_cfm = finaldesc.SoftCFM;
				contact[i].surface.bounce = finaldesc.Bounce;
					contact[i].surface.bounce_vel = finaldesc.BounceVelocity ;
				dJointID c = dJointCreateContact (solver->WorldID,solver->ContactGroup,&contact[i]);


				//Insert friction anisotropy code here
				fixFrictionVector( b1, b2, contact[i] );


				dJointAttach (c,
						dGeomGetBody(contact[i].geom.g1),
						dGeomGetBody(contact[i].geom.g2));
			}
				


		}
	}

}

DynamicsSolver::DynamicsSolver()
{
	IsSetup = false;
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		ConstructionList[i] = NULL;
	}

	for(int j=0; j<MAX_STRUCTURES; j++)
	{
		StructureList[j] = NULL;
	}

	for(int n=0; n<CONSTRUCTION_CACHE_SIZE; n++)
	{
		ActiveConstructionCache[n] = NULL;
	}

	StaticSpaceID = NULL;
	SpaceDist = NULL;

	nConstructions = 0;
	nStructures = 0;

	

}
DynamicsSolver::~DynamicsSolver()
{
	Shutdown();
}


bool DynamicsSolver::Init( int spaceDim, Real spaceWidth, Real spaceHeight)
{
	Shutdown();
	spaceDim = 1;

	WorldID = dWorldCreate();
	DynamicSpaceID = dHashSpaceCreate(0);
	ActiveSpaceID  = dSimpleSpaceCreate(0);
	dSpaceSetCleanup ( ActiveSpaceID, 0 );
	dSpaceSetCleanup ( DynamicSpaceID, 0 );

	SpaceDim    = spaceDim;
	SpaceWidth  = spaceWidth;
	SpaceHeight = spaceHeight;

	nCachedConstructions = 0;
	

	StaticSpaceID  = new dSpaceID[spaceDim*spaceDim];
	SpaceDist = new int[spaceDim*spaceDim];
	for (int i=0; i<spaceDim*spaceDim; i++)
	{
		StaticSpaceID[i]  = dHashSpaceCreate(0);
		//dVector3 center = { spaceWidth*.5f, 0, spaceHeight*.5f, 1.0f };
		//dVector3 extent = { spaceWidth, 10000, spaceHeight, 1.0f };
		//StaticSpaceID[i] = dQuadTreeSpaceCreate(NULL, center, extent, 4);
		SpaceDist[i] = 0;
	}

	ContactGroup = dJointGroupCreate (0);
	dWorldSetGravity (WorldID,0,-90, 0);
	//dWorldSetERP (WorldID, 0.9 );
	dWorldSetContactSurfaceLayer (WorldID, .01);

	IsSetup = true;

	return true;
}

void DynamicsSolver::Shutdown()
{
	if(IsSetup)
	{
		IsSetup = false;
		dWorldDestroy (WorldID);
	}
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		if(ConstructionList[i])
			delete ConstructionList[i];
		ConstructionList[i] = NULL;
	}

	for(int j=0; j<MAX_STRUCTURES; j++)
	{
		if( StructureList[j] )
			delete StructureList[j];
		StructureList[j] = NULL;
	}

	if(StaticSpaceID)
		delete [] StaticSpaceID;
	StaticSpaceID = NULL;

	if(SpaceDist)
		delete [] SpaceDist;
	SpaceDist = NULL;

	nConstructions = 0;
	nStructures    = 0;


}

//Find a place in the list to store the construction, or return
//-1 if nothing could be found
ConstructionHandle DynamicsSolver::RegisterConstruction( Construction* construction)
{
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		if(ConstructionList[i] == NULL)
		{
			ConstructionList[i] = construction;
			nConstructions++;
			return i;
		}
	}

	return -1;
}

//find a place in the list to store the structure, or return -1 if there is no place to store it
StructureHandle    DynamicsSolver::RegisterStructure( Structure* structure)
{
	for(int i=0; i<MAX_STRUCTURES; i++)
	{
		if(StructureList[i] == NULL)
		{
			StructureList[i] = structure;
			StructureList[i]->Registered = true;
			nStructures++;
			return i;
		}
	}

	return -1;
}

void DynamicsSolver::DestroyStructure( StructureHandle h )
{
	if(h <0 || h >= MAX_STRUCTURES )
		return;

	if(StructureList[h])
	{
		nStructures--;
		delete StructureList[h];
	}
	StructureList[h] = NULL;
}

void DynamicsSolver::DestroyConstruction( ConstructionHandle h )
{
	if(h <0 || h >= MAX_CONSTRUCTIONS  )
		return;

	if(ConstructionList[h])
	{
		nConstructions--;
		delete ConstructionList[h];
	}
	ConstructionList[h] = NULL;
}

void DynamicsSolver::DestroyAllObjects()
{
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
		DestroyConstruction(i);
	for(int i=0; i<MAX_STRUCTURES; i++)
		DestroyStructure(i);
}

void DynamicsSolver::GetTransform ( dGeomID geom, Matrix* M )
{
	float outmatrix[16];
	dReal* L; dReal* R;
	L  = (dReal*)dGeomGetPosition(geom);
	R  = (dReal*)dGeomGetRotation(geom);

	outmatrix[0]=(float)R[0];
	outmatrix[1]=(float)R[4];
	outmatrix[2]=(float)R[8];
	outmatrix[3]=0;
	outmatrix[4]=(float)R[1];
	outmatrix[5]=(float)R[5];
	outmatrix[6]=(float)R[9];
	outmatrix[7]=0;
	outmatrix[8]=(float)R[2];
	outmatrix[9]=(float)R[6];
	outmatrix[10]=(float)R[10];
	outmatrix[11]=0;
	outmatrix[12]=(float)L[0];
	outmatrix[13]=(float)L[1];
	outmatrix[14]=(float)L[2];
	outmatrix[15]=1;
	memcpy( M->Mat, outmatrix, sizeof(float) * 16);
}


void DynamicsSolver::GetTransform ( dBodyID geom, Matrix* M )
{
	float outmatrix[16];
	dReal* L; dReal* R;
	L  = (dReal*)dBodyGetPosition(geom);
	R  = (dReal*)dBodyGetRotation(geom);

	outmatrix[0]=(float)R[0];
	outmatrix[1]=(float)R[4];
	outmatrix[2]=(float)R[8];
	outmatrix[3]=0;
	outmatrix[4]=(float)R[1];
	outmatrix[5]=(float)R[5];
	outmatrix[6]=(float)R[9];
	outmatrix[7]=0;
	outmatrix[8]=(float)R[2];
	outmatrix[9]=(float)R[6];
	outmatrix[10]=(float)R[10];
	outmatrix[11]=0;
	outmatrix[12]=(float)L[0];
	outmatrix[13]=(float)L[1];
	outmatrix[14]=(float)L[2];
	outmatrix[15]=1;
	memcpy( M->Mat, outmatrix, sizeof(float) * 16);
}

void DynamicsSolver::GetRotationMatrix( dGeomID geom, Matrix* M )
{
	float outmatrix[16];
	dReal* R;
	R  = (dReal*)dGeomGetRotation(geom);
	outmatrix[0]=(float)R[0];
	outmatrix[1]=(float)R[4];
	outmatrix[2]=(float)R[8];
	outmatrix[3]=0;
	outmatrix[4]=(float)R[1];
	outmatrix[5]=(float)R[5];
	outmatrix[6]=(float)R[9];
	outmatrix[7]=0;
	outmatrix[8]=(float)R[2];
	outmatrix[9]=(float)R[6];
	outmatrix[10]=(float)R[10];
	outmatrix[11]=0;
	outmatrix[12]=0;
	outmatrix[13]=0;
	outmatrix[14]=0;
	outmatrix[15]=1;
	memcpy( M->Mat, outmatrix, sizeof(float) * 16);

}


void DynamicsSolver::GetRotationMatrix( dBodyID geom, Matrix* M )
{
	float outmatrix[16];
	dReal* R;
	R  = (dReal*)dBodyGetRotation(geom);
	outmatrix[0]=(float)R[0];
	outmatrix[1]=(float)R[4];
	outmatrix[2]=(float)R[8];
	outmatrix[3]=0;
	outmatrix[4]=(float)R[1];
	outmatrix[5]=(float)R[5];
	outmatrix[6]=(float)R[9];
	outmatrix[7]=0;
	outmatrix[8]=(float)R[2];
	outmatrix[9]=(float)R[6];
	outmatrix[10]=(float)R[10];
	outmatrix[11]=0;
	outmatrix[12]=0;
	outmatrix[13]=0;
	outmatrix[14]=0;
	outmatrix[15]=1;
	memcpy( M->Mat, outmatrix, sizeof(float) * 16);

}

dSpaceID DynamicsSolver::GetSpaceID( bool Static, Real x, Real y, Real z) 
{

	if(Static)
	{
		int lx, lz;
		lx = (int)(x/(SpaceWidth/(float)SpaceDim));
		lz = (int)(z/(SpaceHeight/(float)SpaceDim));

		if(lx < 0 )
			lx = 0;
		if( lx >= SpaceDim )
			lx = SpaceDim -1;
		if( lz < 0)
			lz = 0;
		if( lz >= SpaceDim )
			lz = SpaceDim -1;

		SpaceDist[lx+lz*SpaceDim] ++;
		return StaticSpaceID[lx+lz*SpaceDim];
	}
	
	return DynamicSpaceID; 
}

Construction* DynamicsSolver::GetConstruction(ConstructionHandle h) 
{
	if(h>=MAX_CONSTRUCTIONS)
		return NULL;
	if(h<0)
		return NULL;

	return ConstructionList[h];
}


Structure* DynamicsSolver::GetStructure(StructureHandle h) 
{
    if(h>=MAX_STRUCTURES)
		return NULL;
	if(h<0)
		return NULL;

	return StructureList[h];
}


bool DynamicsSolver::CheckParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo * c, MeshManager* mmptr )
{
	START_TIMING


	//test constructions
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		if(ConstructionList[i])
		{
			if( ConstructionList[i]->CheckForParticleCollision( p1, p2, c, mmptr) )
			{
				STOP_TIMING
				return true;
			}
		}
	}

	//Test structures
/*	for(int i=0; i<MAX_STRUCTURES; i++)
	{
		if(StructureList[i])
		{
			if( StructureList[i]->CheckForParticleCollision( p1, p2, c, mmptr) )
			{
				STOP_TIMING
				return true;
			}
		}
	}*/

	STOP_TIMING
	return false;
	
}


void DynamicsSolver::DisableInactiveConstructions()
{
	START_TIMING

	//Declarations
	Vector3D v, v2;
	Real     SquareMag;


	//first just do a quick loop through and disable any bodies that need disabling
	int i2=0;
	for(int i=0; i<MAX_CONSTRUCTIONS && i2<nConstructions; i++)
	{
		if(ConstructionList[i])
			ConstructionList[i]->Disable();
	}


	//Now loop through all the constructions and find out which ones
	//we can disable the geoms on
	i2=0;
	bool disablegeoms=true;
	for(int i=0; i<MAX_CONSTRUCTIONS && i2<nConstructions; i++)
	{
		if(ConstructionList[i])
		{
			i2++;
			//Check to make sure the body is disabled
			if( !ConstructionList[i]->IsActive() )
			{
				disablegeoms=true;

				//body is disabled, so check if we can disable the geom too
				ConstructionList[i]->GetLocation( v );

					
				
				//Check if there are any other *active* constructions nearby -- if there are, activate
				//the geoms and break from the loop
				int j2=0;
				for(int j=0; j<CONSTRUCTION_CACHE_SIZE && j2<nCachedConstructions; j++)
				{
					if(ActiveConstructionCache[j])
					{	
						j2++;

						//only interested in active constructions
						if(ActiveConstructionCache[j]->IsActive() == false)
						{
							continue;
						}
						
						
						//Get the location and figure out the distance between the two constructions
						ActiveConstructionCache[j]->GetLocation(v2 );
						SquareMag = (v2.x-v.x)*(v2.x-v.x) + (v2.z-v.z)*(v2.z-v.z);



						//if the distance between the two constructions is small enough, enable them
						if(SquareMag < 10000)
						{
							//[HACK] this is a horrible horrible hack, I shouldn't use a 
							//constant here at all. I'm too lazy to calculate radius's in
							//all the bodies at the moment though


							//something close by, so re-enable all the geom's and break
							ConstructionList[i]->EnableAllGeoms();
							disablegeoms=false;
							break;
						}
						
					}
				}
				
				//nothing nearby, disable the geom
				if(disablegeoms)
					ConstructionList[i]->DisableAllGeoms();
				

			}


		}
	}

	
	STOP_TIMING
}

void DynamicsSolver::DisableInactiveStructures()
{
	
	START_TIMING
	
	//declarations
	Real SquareMag;
	dReal* v1;
	dReal* v2;
	int n=0, n2=0;
	int i=0, i2=0;
	//loop through the structures, checking which ones should be disabled
	while( i<MAX_STRUCTURES && i2<nStructures)
	{
		if(StructureList[i])
		{
			i2++;
			if(StructureList[i]->NoDisable )
			{
				i++;
				continue;
			}

			//disable the thingy and such and such	
			dGeomDisable( StructureList[i]->Geom );

			//loop through constructions and find the 
			//distance of each of them from our structure.
			//if any of them are close enough we'll enable
			//the structure
					

			n =0;
			n2=0;
			while( n<CONSTRUCTION_CACHE_SIZE && n2<nCachedConstructions )
			{
				
				if(ActiveConstructionCache[n])
				{
					if(ActiveConstructionCache[n]->IsActive() )
					{
					
						v1 = (dReal*)dBodyGetPosition( ActiveConstructionCache[n]->GetSubObject(0)->Body );
						v2 = (dReal*)dGeomGetPosition( StructureList[i]->Geom );

						//Get the square magnitude
						SquareMag = (v2[0]-v1[0])*(v2[0]-v1[0]) +  (v2[2]-v1[2])*(v2[2]-v1[2]) ;
	

						if( SquareMag <  StructureList[i]->Radius*StructureList[i]->Radius + 10000 )
						{
							//enable the structure
							dGeomEnable( StructureList[i]->Geom );
							break; // break out of the loop since we enabled the structure
						}

					}
					
					n2++;

				}
				n++;
			}
		}
		i++;
	}

	STOP_TIMING


 }


void DynamicsSolver::Step( float FrameTime, WorldManager* WM )
{
	START_TIMING
	

	START_TIMING_BLOCK( "GarbageCollectCache" );
	GarbageCollectCache();
	STOP_TIMING_BLOCK( "GarbageCollectCache" );


	START_TIMING_BLOCK( "UpdateConstructions" );

	//update all the objects
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		if(ConstructionList[i])
			ConstructionList[i]->MainUpdate( FrameTime, WM );
	}

	STOP_TIMING_BLOCK( "UpdateConstructions" );
	
	
	START_TIMING_BLOCK( "dSpaceCollide" );
    //collide the dynamic spaces with themselves
    dSpaceCollide (ActiveSpaceID,this,&nearCallback);
	

	STOP_TIMING_BLOCK( "dSpaceCollide" );


	START_TIMING_BLOCK( "ActiveSpaceCollide" );
	dSpaceCollide2( (dxGeom *)ActiveSpaceID, (dxGeom *)DynamicSpaceID, this, &nearCallback );
	STOP_TIMING_BLOCK( "ActiveSpaceCollide" );

	//collide the dynamic space with the static spaces
	START_TIMING_BLOCK( "dSpaceCollide2" );
	for(int i=0; i<SpaceDim*SpaceDim; i++)
		dSpaceCollide2 ( (dxGeom *)ActiveSpaceID, (dxGeom *)StaticSpaceID[i], this, &nearCallback );
	STOP_TIMING_BLOCK ( "dSpaceCollide2" );


	START_TIMING_BLOCK( "dWorldStep");
	dWorldStep(WorldID, FrameTime);
	//dWorldSetQuickStepNumIterations ( WorldID, 10 );
	//dWorldQuickStep( WorldID, FrameTime);
	//dWorldStep( WorldID, FrameTime );
	//dWorldStepFast1(WorldID, FrameTime, 5);
	STOP_TIMING_BLOCK( "dWorldStep");

	// remove all contact joints
	dJointGroupEmpty (ContactGroup);


	DisableInactiveConstructions();
	DisableInactiveStructures ();

	
	STOP_TIMING
}

void DynamicsSolver::DrawAll( Screen3D& Screen )
{
	START_TIMING
	int i2 = 0;
	for(int i=0; i<MAX_CONSTRUCTIONS; i++)
	{
		if(i2>nConstructions)
			break;
	
		if(ConstructionList[i])
		{
			ConstructionList[i]->Draw( Screen );
			i2++;
		}
	}
	STOP_TIMING

}


//stuff for active construction cache
void DynamicsSolver::CacheConstruction ( Construction* c )
{
	//first make sure it isn't already in the cache
	bool Done=false;
	int i2=0;
	for(int i=0; i<CONSTRUCTION_CACHE_SIZE && i2 < nCachedConstructions; i++)
	{
		if(ActiveConstructionCache[i])
			i2++;
		
		if( ActiveConstructionCache[i] == c )
			return;
	}


	//find and empty spot in our cache to place construction pointer
	for(int i=0; i<CONSTRUCTION_CACHE_SIZE && Done == false; i++)
	{
		

		if( ActiveConstructionCache[i] == NULL )
		{
			ActiveConstructionCache [i] = c; 
			CacheSpaceAddConstruction( c );
			Done = true;
			nCachedConstructions++;
		}
				

		
	}
}

void DynamicsSolver::GarbageCollectCache ()
{
	//test if any of the objects should be disabled, if they 
	//should be then garbage collect them and stuff
	for(int i=0; i<CONSTRUCTION_CACHE_SIZE ; i++)
	{
		if( ActiveConstructionCache[i])
		{
			if(! ActiveConstructionCache [i]->IsActive () ) 
			{
				CacheSpaceRemoveConstruction( ActiveConstructionCache[i] );
				ActiveConstructionCache[i] = NULL;
				nCachedConstructions--;
			}

		}
	}
}

void DynamicsSolver::CacheSpaceAddConstruction( Construction* c)
{
	dSpaceRemove( DynamicSpaceID,(dGeomID)c->GeomGroup );
	dSpaceAdd( ActiveSpaceID,  (dGeomID)c->GeomGroup );
}

void DynamicsSolver::CacheSpaceRemoveConstruction( Construction* c )
{

	dSpaceRemove( ActiveSpaceID,(dGeomID) c->GeomGroup );
	dSpaceAdd( DynamicSpaceID,  (dGeomID)c->GeomGroup );

}