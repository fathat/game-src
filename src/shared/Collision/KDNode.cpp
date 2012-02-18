#include "KDNode.h"
#include "KDTree.h"
#include "..\world\sceneobject.h"

KDNode::~KDNode() 
{
	if( front )
		delete front;
	if( back )
		delete back;

}

void KDNode::BuildTree( KDTree* tree,  int currentDepth, AABB bb, int axis )
{

	//store the bounding box
	boundingBox = bb;

    //figure out direction of split (horizontal if axis = 0, vertical if axis = 1)
	if( axis == 0 )
	{
		splitPlane = Plane3D( 0, 0, 1, bb.min.z + (bb.max.z - bb.min.z)/2  );	

	}
	else if (axis == 1 )
	{
		splitPlane = Plane3D( 1, 0, 0, bb.min.x + (bb.max.x - bb.min.x)/2  );	
	}


	if(currentDepth < tree->MaxDepth() )
	{

		//Split bounding box into two smaller bounding boxes
		AABB b1, b2;
		b1 = b2 = bb;

		if( axis == 0)
		{
			b1.max.z = bb.min.z + (bb.max.z - bb.min.z)/2;
			b2.min.z = bb.min.z + (bb.max.z - bb.min.z)/2;
		}
		else
		{
			b1.max.x = bb.min.x + (bb.max.x - bb.min.x)/2;
			b2.min.x = bb.min.x + (bb.max.x - bb.min.x)/2;
			
		}


		//Set the axis
		if( axis == 0 )
			axis = 1;
		else 
			axis = 0;

        //Build the two sub trees		
		front = new KDNode( tree, this );
		back = new KDNode( tree, this );

		back->BuildTree( tree, currentDepth+1, b1, axis );
		front->BuildTree( tree, currentDepth+1, b2, axis );

	}
	

}


//Recurses the tree and fills a list with the objects that might potentially collide with the provided ray
void KDNode::GetPotentialColliders( Vector3D& a, Vector3D& b,  std::vector< KDObjectContainer*>& colliderList )
{

	//first, check if this is leaf node. If it is, put its objects into the collider list.
	if( front == NULL && back == NULL )
	{
		for( int i=0; i<objectList.size() ; i++ )
			colliderList.push_back( objectList[i] );

		return;
		
	}


	//record the respective distance of each point from the plane 
	float d_a = (a.x * splitPlane.a() + a.y * splitPlane.b() + a.z * splitPlane.c()) - splitPlane.d();
	float d_b = (b.x * splitPlane.a() + b.y * splitPlane.b() + b.z * splitPlane.c()) - splitPlane.d();

	//Figure out which side point a is on 
	if(  d_a > 0 ) //Front side check
	{
		if ( d_b <= 0 ) //if point b is on the back side of the splitting plane, we have to check
						//both front and back
		{
			front->GetPotentialColliders( a, b, colliderList );
			back->GetPotentialColliders( a, b, colliderList );
			return;
		}
		else //otherwise, just check front since both points are on the same side
		{
			front->GetPotentialColliders( a, b, colliderList);
			return;
		}

	}
	else //back side check
	{
		if( d_b >= 0) //Point b is on the front side of the split plane,
					  // so check both front and back (starting at back)
		{
			back->GetPotentialColliders( a, b, colliderList );
			front->GetPotentialColliders( a, b, colliderList );
			return;
		}
		else
		{
			back->GetPotentialColliders( a, b, colliderList );
		}
	}


}

void KDNode::AddObject( KDObjectContainer* container )
{
	//check if this is a leaf. If it is, add the object and return
	if( front == NULL )
	{
		objectList.push_back(container );
		return;
	}

	//otherwise, determine which side of the splitting plane the object
	//is on and add it to that side (or if its on the plane, add it to both sides).
	container->object->AdjustBoundingVolume();
	BoundingSphere bs = container->object->GetBoundingSphere();
	

	float d = (bs.x() * splitPlane.a() + bs.y() * splitPlane.b() + bs.z() * splitPlane.c()) - splitPlane.d();

	if( fabs( d ) <= bs.Radius )
	{
		//on the plane, add to both sides
		front->AddObject( container );
		back->AddObject( container );
	}

	else if ( d > bs.Radius )
	{
		//add it to the front of the plane
		front->AddObject( container );
	}

	else if ( d < bs.Radius )
	{
		//add it to the back of the plane
		back->AddObject( container );
	}
	
}