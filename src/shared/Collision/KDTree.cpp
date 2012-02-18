#include "KDTree.h"
#include "..\world\sceneobject.h"

//Resets all objects to "not checked"
void KDTree::ClearCheckedFlag()
{
	//clear all checked flags
	std::map<SceneObject*, KDObjectContainer*>::const_iterator i;
	for ( i = objects.begin( ); i != objects.end( ); i++ )
	{
		
		(i->second)->ClearChecked();
	}
}

//Initializes the KDTree
void KDTree::Init( float width, float depth, int aMaxDepth )
{
	//cleanup any old resources
	Cleanup();

	//record the maximum depth of the tree
	maxDepth = aMaxDepth;
    
	//store the boundaries
	AABB boundaries( Vector3D ( 0, 0, 0 ), Vector3D ( width, 0, depth ) );
	
	//build the tree
	root = new KDNode( this, NULL  );
	root->BuildTree( this, 0, boundaries, 0 );
}



//Deletes all the resources the KDTree is using
void KDTree::Cleanup()
{
	if( root )
		delete root;
	root = NULL;

	//delete all the object containers
	std::map<SceneObject*, KDObjectContainer*>::const_iterator i;
	for ( i = objects.begin( ); i != objects.end( ); i++ )
	{

		KDObjectContainer* kdobject = (i->second);
		delete kdobject;
		objects[(i->first)] = NULL;
		
	}

	objects.clear();

	
}

//Adds an object to the KD tree
void KDTree::Add( SceneObject* sceneObject )
{
	//Add the object to the listing of objects
	objects[sceneObject] = new KDObjectContainer( sceneObject );

	root->AddObject( objects[sceneObject ]);
}


//Removes an object from the KD tree
void KDTree::Remove( SceneObject* object)
{
	
	objects.erase( object );
}

//Finds the closest object that intersects the line segment defined by ab. Returns false if nothing found.
bool KDTree::CastRay( Vector3D& a, Vector3D& b, Vector3D* outPos, Vector3D* outNormal, SceneObject** outObject )
{

	//Make everything unchecked
	ClearCheckedFlag();

	//Get all the potential colliders
	std::vector< KDObjectContainer*> colliderList;
	root->GetPotentialColliders( a, b, colliderList );

	//check with each of those for an actual collision
    if( colliderList.size() > 0 )
	{
		SceneObject* closestCollider= NULL;
		SceneObject* o;
		CollisionInfo closestInfo;
		
		for( int i=0; i<colliderList.size() ; i++ )
		{
			o = colliderList[i]->object;
			
			if( colliderList[i]->Checked()  )
				continue; 
			
			CollisionInfo info;
			if( o->Collision( a, b, &info ) )
			{
				if( closestCollider == NULL)
				{
					closestCollider = o;
					closestInfo = info;
				}
				else
				{
					Vector3D da = closestInfo.CollisionPoint - a;
					Vector3D db = info.CollisionPoint - a;

					if( db.MagSq() < da.MagSq() )
					{
						closestCollider = o;
						closestInfo = info;
					}
				}

			}

			colliderList[i]->SetChecked();
		}


		if( closestCollider != NULL)
		{
			if( outObject != NULL )
				*outObject = o;
			if( outPos != NULL )
				*outPos = closestInfo.CollisionPoint;
			if( outNormal != NULL )
				*outNormal = closestInfo.PlaneNormal;
			return true;
		}
		//*outObject = colliderList[0]->object;
				
	}

	return false;
}