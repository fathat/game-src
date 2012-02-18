#include "quadtree.h"
#include "world.h"

#define GRASS_DENSITY 16000

QuadNode::QuadNode() { 
	TopLeft  = NULL; 
	TopRight = NULL; 
	BotLeft  = NULL; 
	BotRight = NULL; 
	MapVB	 = NULL; 
	Patch	 = NULL;
	Invisible=false; 

}
QuadNode::~QuadNode() { 
	Cleanup();
}


bool QuadNode::InitializeGeometry( Terrain* terrain)
{

	m_terrain = terrain;
	//First, build the set of vertices for the geometry and
	//calculate things like bounding boxes
	LandscapeVertex* QuadVertices;
	QuadVertices = new LandscapeVertex[terrain->GetNodeWidth()*terrain->GetNodeHeight()];


	int mapx;
	int mapy;
	Real MinY= 10000;
	Real MaxY=-10000;
	int CurrentWaterIndex=0;

	//Go through every point and fill it in
	for(int y=0; y<terrain->GetNodeHeight(); y++)
	{
		for(int x=0; x<terrain->GetNodeWidth(); x++)
		{
			mapx = x+(Box.min.x/terrain->GetSpacing());
			mapy = y+(Box.min.z/terrain->GetSpacing());
			
			if(mapx+1 >= terrain->GetWidth() ) mapx = terrain->GetWidth() -2;
			if(mapy+1 >= terrain->GetHeight()) mapy = terrain->GetHeight()-2;

			//fill in vertex data
			QuadVertices[x+y*terrain->GetNodeWidth()].position.x = (mapx*terrain->GetSpacing());
			QuadVertices[x+y*terrain->GetNodeWidth()].position.y = terrain->GetHeightmapArray()[(mapx+(mapy*terrain->GetWidth()))];
			QuadVertices[x+y*terrain->GetNodeWidth()].position.z = (mapy*terrain->GetSpacing());
			QuadVertices[x+y*terrain->GetNodeWidth()].normal.y   = 1;
			QuadVertices[x+y*terrain->GetNodeWidth()].tu         = (Real) x/((Real)terrain->GetNodeWidth()/2);
			QuadVertices[x+y*terrain->GetNodeWidth()].tv         = (Real) y/((Real)terrain->GetNodeWidth()/2);
			QuadVertices[x+y*terrain->GetNodeWidth()].tu2        = (Real) x/((Real)terrain->GetNodeWidth());
			QuadVertices[x+y*terrain->GetNodeWidth()].tv2        = (Real) (Real)((Real)y/((Real)terrain->GetNodeHeight()));

			if(terrain->GetHeightmapArray()[(mapx+(mapy*terrain->GetWidth()))]>MaxY)
				MaxY = terrain->GetHeightmapArray()[(mapx+(mapy*terrain->GetWidth()))];
			if(terrain->GetHeightmapArray()[(mapx+(mapy*terrain->GetWidth()))]<MinY)
				MinY = terrain->GetHeightmapArray()[(mapx+(mapy*terrain->GetWidth()))];

			//Calculate the normal
			Vector3D A, B, R;

			A.x = (Real) terrain->GetSpacing();
			A.y = terrain->GetHeightmapArray()[(mapx+1)+mapy*terrain->GetWidth()] - terrain->GetHeightmapArray()[mapx+mapy*terrain->GetWidth()];
			A.z = 0;
			B.x = 0;
			B.y = terrain->GetHeightmapArray()[mapx+((mapy+1)*terrain->GetWidth())] - terrain->GetHeightmapArray()[mapx+mapy*terrain->GetWidth()];
			B.z = (Real) terrain->GetSpacing();

			//Normal is a cross between A and B
			R = B.Cross(A);
			R.Normalize();
			QuadVertices[x+y*terrain->GetNodeWidth()].normal.x = R.x;
			QuadVertices[x+y*terrain->GetNodeWidth()].normal.y = R.y;
			QuadVertices[x+y*terrain->GetNodeWidth()].normal.z = R.z;
			
			Vector3D R2;
            //Figure out the normal for the second triangle
			if(y<terrain->GetNodeHeight()-1 && x<terrain->GetNodeWidth()-1)
			{
				A.x = 0;
				A.y =  terrain->GetHeightmapArray()[(mapx+1)+(mapy+1)*terrain->GetWidth()] - terrain->GetHeightmapArray()[mapx+(mapy+1)*terrain->GetWidth()];
				A.z = (Real) -terrain->GetSpacing();
				B.x = (Real) -terrain->GetSpacing();
				B.y = terrain->GetHeightmapArray()[(mapx+1)+(mapy+1)*terrain->GetWidth()] - terrain->GetHeightmapArray()[(mapx+1)+mapy*terrain->GetWidth()];
				B.z = 0;

				R2 = B.Cross(A);			
				R2.Normalize();
			}
		}
	}


	//Set the extents of the bounding box
	Box.max.y = MaxY;
	Box.min.y = MinY;

	//Now that we have indices and such we need to make the index buffers
	
	

	//Hooray! The game hasn't crashed yet!
	return true;

}

void QuadNode::InitGrass()
{
	m_grass.Init( m_terrain->GetScreen(), this, GRASS_DENSITY  );
}

void QuadNode::Cleanup()
{

	if(TopLeft) TopLeft->Cleanup(); 
	if(TopRight) TopRight->Cleanup();
	if(BotLeft) BotLeft->Cleanup();
	if(BotRight) BotRight->Cleanup();

	if(MapVB) delete MapVB;
	MapVB= NULL;
}

Vector3D QuadNode::Center()
{
	float x = (Box.max.x - Box.min.x)/2 + Box.min.x;
	float y = (Box.max.y - Box.min.y)/2 + Box.min.y;
	float z = (Box.max.z - Box.min.z)/2 + Box.min.z;
	return Vector3D( x, y, z );
}

float QuadNode::DiagonalSize()
{
	float dx = Box.max.x - Box.min.x;
	float dz = Box.max.z - Box.min.z;
	return sqrt( (dx/2)*(dx/2) + (dz/2)*(dz/2) );
}