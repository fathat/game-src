#include "scenemanager.h"
#include "SphereTree.h"
#include "world.h"
#include "..\collision\collision.h"
#include "..\collision\collisionutils.h"


#include "..\defs\defs.h"

void SphereNode::SetupAsRoot( SceneManager* scene )
{
	//grab all the objects from scene manager
	for( SceneManagerIterator i = scene->Start(); i!=scene->End(); i++)
	{
		objectList.push_back( i->first );
	}

}

void SphereNode::Build( SceneManager* scene, SphereNode* parent, bool splitZ, int currentDepth )
{
	//Calculate the midpoint location. We use this to split object list
	float median=0.0f;
	for( int i=0; i<objectList.size(); i++)
	{
		SceneObject* so = scene->GetObjectPtr( objectList[i] );
		if (!splitZ)
			median += so->GetX();
		else
			median += so->GetZ();
	}

	median /= objectList.size();

	//Create child nodes
	left = new SphereNode();
	right = new SphereNode();

	//classify objects and assign them to respective nodes
	for( int i=0; i<objectList.size(); i++)
	{
		SceneObject* so = scene->GetObjectPtr( objectList[i] );
		if (!splitZ)
		{
			if ( so->GetX() < median )
				left->AddObject( objectList[i] );
			else
				right->AddObject( objectList[i] );
		}
		else
		{
			if ( so->GetZ() < median )
				left->AddObject( objectList[i] );
			else
				right->AddObject( objectList[i] );
		}
	}

	//figure out bounding box
	CalculateExtents( scene );

	//Build next level
	if( currentDepth < scene->GetMaxDepth() )
	{
		left->Build( scene, this, !splitZ, currentDepth+1 );
		right->Build( scene, this, !splitZ, currentDepth+1 );
	}
}

void SphereNode::CalculateExtents( SceneManager* scene)
{
	//figure out the center point
	location = Vector3D(0,0,0);
	for( int i=0; i<objectList.size(); i++)
	{
		SceneObject* so = scene->GetObjectPtr( objectList[i] );
		location += so->GetLocation().GetVector();
	}

	location *= 1.0f/(float)objectList.size();

	//Now figure out extents of sphere
	Vector3D t;
	float d;
	radius = 0.0f;
	for( int i=0; i<objectList.size(); i++)
	{
		SceneObject* so = scene->GetObjectPtr( objectList[i] );
		t = so->GetLocation().GetVector() - location;

		d = t.Mag()+so->GetBoundingSphere().Radius;

		if( d > radius)
			radius = d; 
	}

}

bool SphereNode::Collide( SceneManager* scene,  Vector3D* l1, Vector3D* l2, CollisionInfo* c )
{
	if( CollisionUtil::LineIntersectSphere( *l1, *l2, this->location, this->radius ) )
	{
		if( left && right)
		{
			
			if( left->Collide( scene,  l1, l2, c ) )
				return true;

			if( right->Collide(scene,  l1, l2, c ) )
				return true;
		}
		else
		{
			//leaf node, check all the objects
			for( int i=0; i<objectList.size(); i++)
			{
				SceneObject* so = scene->GetObjectPtr(objectList[i]);

				if( so )
				{
					if( so->Collision( *l1, *l2, c ) )
						return true;
				}

			}
		}

	}
	return false;
}