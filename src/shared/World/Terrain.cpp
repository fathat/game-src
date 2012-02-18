#include "Terrain.h"
#include "World.h"
#include "..\CVAR\CVAR.h"
//#include "..\settings\settings.h"


void Terrain::SetupDefaultValues()
{
	Map = NULL;
	NodeList = NULL;
	Tree     = NULL;

	Width    = 0;
	Height   = 0;
	Spacing  = 0;
	TerrainGeom = NULL;
	NodeWidth  = 0;
	NodeHeight = 0;
	nAcross = 0;
	nDown   = 0;

	LowestHeight  = 0;
	HighestHeight = 0;

	CurrentIndex = 0;
	CurrentDepth = 0;
	MaxDepth     = 4;

	TerrainShader = NULL;
	nNodes = 0;

	NodeReferenceList = 0;

	terrainMap = NULL;

	DrawGrass = true;

}

Terrain::Terrain( WorldManager* wm, const char* heightmap, const char* layerfile) 
{
	SetupDefaultValues();

	//store pointer to world manager
	WMPtr = wm;
	ScreenPtr = wm->ScreenPtr;
	Spacing = wm->Spacing;

	CVar.WriteLine( "Loading Heightmap" );
	LoadHeightmapDotBin( heightmap );
	CVar.WriteLine( "Loading Texture Layers" );
	LoadTextureLayers( layerfile );
	
	
	CVar.WriteLine( "Initializing LOD terrain map" );
	terrainMap = new TerrainMap( ScreenPtr, heightGrid, 17 );
	terrainMap->Spacing( Spacing );
	
    
	MaxDepth = log((float) Width ) / log((float)2) - log( (float)17-1) / log((float)2) + 1;
	//determine what the size of the deepest level nodes will be
	NodeWidth  = Width;
	NodeHeight = Height;

	int div;
	for( div=0; div<MaxDepth-1; div++)
	{
		NodeWidth /= 2;
	}
	for(div=0; div<MaxDepth-1; div++)
	{
		NodeHeight /= 2;
	}
	
	//NodeWidth  = 17;
	//NodeHeight = 17;

	

	nAcross = Width/NodeWidth;
	nDown   = Height/NodeHeight;

	NodeReferenceList = new QuadNodePtr[nAcross*nDown];

	NodeWidth++;
	NodeHeight++;


	//construct an index buffer to use with quad nodes
	WORD* IndexList;

	CVar.WriteLine( "Initializing obsolete index buffer" );
	IB.Init(*ScreenPtr, ((NodeWidth-1)*(NodeHeight-1)*6), IB16Bit);
	IB.Lock(&IndexList);

	int IndexOffset=0;

	for(int y=0; y<NodeHeight-1; y++)
	{
		for(int x=0; x<NodeWidth-1; x++)
		{
			IndexList[IndexOffset+0] = x+y*NodeWidth;
			IndexList[IndexOffset+1] = x+((y+1)*NodeWidth);
			IndexList[IndexOffset+2] = (x+1)+y*NodeWidth;
			IndexList[IndexOffset+3] = x+(y+1)*NodeWidth;
			IndexList[IndexOffset+4] = (x+1)+((y+1)*NodeWidth);
			IndexList[IndexOffset+5] = (x+1)+y*NodeWidth;
			IndexOffset += 6;

		}
	}

	IB.Unlock();

	//Build up the tree
	nNodes = (int)((pow(4.0f,(MaxDepth+1))-1)/3);
	NodeList = new QuadNode[nNodes];
	CurrentIndex = 0;

	CVar.WriteLine( "Initializing Quadtree" );
	Tree = new QuadNode;
	BuildTree(NULL, Tree, false, false);

	CVar.WriteLine( "Initializing Terrain Shader" );
	InitTerrainShader();

}

Terrain::~Terrain()
{
	
	delete heightGrid;
	delete terrainMap;
	if(Map)
		delete []Map;

	if(NodeReferenceList)
		delete []NodeReferenceList;
	if(Tree)
		delete Tree;

	//cleanup the layer list
	for ( int i=0; i<LayerList.size(); i++)
	{
		TerrainLayer* l = LayerList[i];
		delete l;
		LayerList[i] = NULL;
	}
	

	LayerList.clear();

	Map				  = NULL;
	NodeReferenceList = NULL;
	Tree			  = NULL;
}

dReal heightfield_callback( void* pUserData, int x, int z )
{
/*	dIASSERT( x < HFIELD_WSTEP );
	dIASSERT( z < HFIELD_DSTEP );

	dReal fx = ( ((dReal)x) - ( HFIELD_WSTEP-1 )/2 ) / (dReal)( HFIELD_WSTEP-1 );
	dReal fz = ( ((dReal)z) - ( HFIELD_DSTEP-1 )/2 ) / (dReal)( HFIELD_DSTEP-1 );*/

	// Create an interesting 'hump' shape
	Terrain* myTerrain = (Terrain*)pUserData;
	//dReal h = 200;
	return myTerrain->GetHeightmapArray()[x+z*myTerrain->GetWidth()];

	//return h;
}


void Terrain::InitializeGeom()
{
	//make a temporary buffer for it since we're using doubles rather than floats
	/*dReal* doubleMap = new dReal[Width*Height];
	for(int i=0; i<Width*Height; i++)
		doubleMap[i] = (dReal )Map[i];
	TerrainGeom = dCreateTerrainY(WMPtr->Solver.GetSpaceID( true, 0, 0, 0 ), doubleMap,Width*Spacing, Width, 0, 0  );
	delete[] doubleMap;*/


	heightid = dGeomHeightfieldDataCreate();

	// Create an finite heightfield.
	
	/*dGeomHeightfieldDataBuildSingle(heightid, 
									Map, FALSE, 
									Width*Spacing, 
									Height*Spacing, 
									Width, 
									Height,
									1.0,
									0.0,
									0.0,
									FALSE);*/
	dGeomHeightfieldDataBuildCallback(heightid,
									  this,
									  heightfield_callback,
									  (Width-1)*Spacing,
									  (Height-1)*Spacing,
									  Width,
									  Height,
									  1.0,
									  0.0,
									  0.0,
									  FALSE);

	// Give some very bounds which, while conservative,
	// makes AABB computation more accurate than +/-INF.
	TerrainGeom = dCreateHeightfield( WMPtr->Solver.GetSpaceID( true, 0, 0, 0 ), heightid, 0 );

}


bool Terrain::LoadTextureLayers( const char* filename )
{
	char buffer[1024];
	sprintf( buffer, "..\\%s\\maps\\%s\\layers.xml", WMPtr->ModName.c_str(), WMPtr->MapName.c_str() );

	//Load document
	TiXmlDocument doc;
	if ( doc.LoadFile( buffer ) == false)
		return false; 

	//read root element
	TiXmlElement * root = doc.RootElement();
	if( root )
	{

		//Read in all layers 
		for( TiXmlElement* node = root->FirstChildElement(  );
			node;
			node = node->NextSiblingElement( ) )
		{

			const char* name		= xmlreads( node, "Name" );
			const char* texturename = xmlreads( node, "TextureName" );

			TerrainLayer *layer =  new TerrainLayer( WMPtr, name, texturename ) ;
			LayerList.push_back( layer );
		}
	}


	return true;

}




bool Terrain::LoadHeightmapDotBin( const char* binfile )
{
	int w, h;
	FILE* file = fopen( binfile, "rb" );
	if ( file == NULL )
		return false;

	fread( &w, 4, 1, file );
	fread( &h, 4, 1, file );

	Width  = w;
	Height = h;

	Map = new float[ Width*Height];
	fread( Map, sizeof(float), Width*Height, file );

	fclose( file );

	LowestHeight = 1000000;
	HighestHeight = -100000;
	
	heightGrid = new HeightGrid	( Width + 1 );
	for(int y=0; y<Height+1; y++)
	{
		for(int x=0; x<Width+1; x++)
		{
			float newheight = 0.0f;
			if( x < Width && y < Height )
				newheight = Map[x+y*Width];
			heightGrid->SetHeight( x, y, newheight );
		}
	}

	//find the lowest and highest height, and set the water map to be flat
	for(int y=0; y<Height; y++)
	{
		for(int x=0; x<Width; x++)
		{
			if( Map[x+(Height-1-y)*Width] < LowestHeight )
				LowestHeight = Map[x+(Height-1-y)*Width];
			if( Map[x+(Height-1-y)*Width] > HighestHeight )
				HighestHeight = Map[x+(Height-1-y)*Width];
			
		}
	}

	


    //huzzah!
	return true;
}



void Terrain::CreateVertexbuffer( QuadNode* Node)
{
	LandscapeVertex* List;

	
	Vector3D* structureVertices = new Vector3D[NodeWidth*NodeHeight];
	Node->MapVB = new VertexBuffer;

	Node->MapVB->Init( *ScreenPtr, D3DFVF_LANDSCAPEVERTEX, sizeof( LandscapeVertex ), NodeWidth * NodeHeight );
	Node->MapVB->Lock((BYTE**)&List);

	int mapx;
	int mapy;

	Real MinY= 100000;
	Real MaxY=-100000;

	Real tx=0;
	Real ty=0;

	Vector3D* nmap;
	nmap = new Vector3D[NodeWidth*NodeHeight];
	
	Vector3D A, B, R;

	for(int y=0; y<NodeHeight; y++)
	{
		for(int x=0; x<NodeWidth; x++)
		{
			mapx = x+(Node->Box.min.x/Spacing);
			mapy = y+(Node->Box.min.z/Spacing);
			
			if(mapx+1 >= Width ) mapx = Width -2;
			if(mapy+1 >= Height) mapy = Height-2;

			//fill in vertex data
			List[x+y*NodeWidth].position.x = (mapx*Spacing);
			List[x+y*NodeWidth].position.y = Map[(mapx+(mapy*Width))];
			List[x+y*NodeWidth].position.z = (mapy*Spacing);
			List[x+y*NodeWidth].normal.y   = 1;
			List[x+y*NodeWidth].tu          = (Real) mapx*.25f;
			List[x+y*NodeWidth].tv          = (Real) mapy*.25f;
			List[x+y*NodeWidth].tu2         = (Real) ((Real)mapx/(Real)Width);
			List[x+y*NodeWidth].tv2         = (Real) ((Real)mapy/(Real)Height);
			List[x+y*NodeWidth].tu3         = (Real) ((Real)mapx/(Real)Width);
			List[x+y*NodeWidth].tv3         = (Real) ((Real)mapy/(Real)Height);
			List[x+y*NodeWidth].tu4         = (Real) ((Real)mapx/(Real)Width);
			List[x+y*NodeWidth].tv4         = (Real) ((Real)mapy/(Real)Height);
			

			structureVertices[x+y*NodeWidth] = List[x+y*NodeWidth].position;

			if(Map[(mapx+(mapy*Width))]>MaxY)
				MaxY = Map[(mapx+(mapy*Width))];
			
			if(Map[(mapx+(mapy*Width))]<MinY)
				MinY = Map[(mapx+(mapy*Width))];
			
		
			if(mapx-1 < 0 ) mapx = 1;
			if(mapy-1 < 0 ) mapy = 1;

			//Calculate the normal
			
			A.x = (Real) Spacing;
			A.y = Map[(mapx+1)+mapy*Width] - Map[mapx+mapy*Width];
			A.z = 0;
			B.x = 0;
			B.y = Map[mapx+((mapy+1)*Width)] - Map[mapx+mapy*Width];
			B.z = (Real) Spacing;
			R = B.Cross(A);
			R.Normalize();
			if(R.y < 0)
				R*=-1.0f;
			
			nmap[x+y*NodeWidth] = R;


			//Set normal in the normal map
			//NormalMap[mapx+mapy*Width] = R;
			
		}
		
	}

	//smooth the normals
	Real DivFactor = 1.0f;
	for(int y=0; y<NodeHeight; y++)
	{
		for(int x=0; x<NodeWidth; x++)
		{
			mapx = x+(Node->Box.min.x/Spacing);
			mapy = y+(Node->Box.min.z/Spacing);
			
			if(mapx+1 >= Width ) mapx = Width -2;
			if(mapy+1 >= Height) mapy = Height-2;
			if(mapx-1 < 0 ) mapx = 1;
			if(mapy-1 < 0 ) mapy = 1;

			
			DivFactor = 1.0f;
			R = nmap[x+y*NodeWidth];

			if( x-1 > 0)
			{
				if(nmap[(x-1)+y*NodeWidth].y < nmap[x+y*NodeWidth].y)
				{
					R += nmap[(x-1)+y*NodeWidth];
					DivFactor += 1.0f;
				}
			}

			if(x+1 < NodeWidth)
			{
				if(nmap[(x+1)+y*NodeWidth].y < nmap[x+y*NodeWidth].y)
				{
					R += nmap[(x+1)+y*NodeWidth];
					DivFactor += 1.0f;
				}
			}
			
			if(y-1 > 0 )
			{
				if(nmap[x+(y-1)*NodeWidth].y < nmap[x+y*NodeWidth].y)
				{
					R += nmap[x+(y-1)*NodeWidth];
					DivFactor += 1.0f;
				}
			}

			if( y+1 < NodeHeight )
			{
				if(nmap[x+(y+1)*NodeWidth].y < nmap[x+y*NodeWidth].y)
				{
					R += nmap[x+(y+1)*NodeWidth];
					DivFactor += 1.0f;
				}
			}
			
		
			
			R /= DivFactor;
			R.Normalize();
			List[x+y*NodeWidth].normal = R;
			//NormalMap[mapx+mapy*Width] = R;
		}	
	}
	
	Node->Box.max.y = MaxY;
	Node->Box.min.y = MinY;
	

	Node->MapVB->Unlock();

	WORD* indices;
	

	Vector3D Location;
	Location.x = 0;
	Location.y = 0;
	Location.z = 0;
	
	IB.Lock(&indices );
	int* notIndices = new int[IB.GetIndexCount()];
    for(int i=0; i<IB.GetIndexCount(); i++)
	{
		notIndices[i] = indices[i];
	}
	
	IB.Unlock();
	delete [] notIndices;
	delete [] structureVertices;

	Node->InitializeGeometry( this );
}


void Terrain::BuildTree( QuadNode* Parent, QuadNode* NewNode, bool Left, bool Top )
{
	// Declarations
	CurrentDepth++;

	// Set up the new node

	//it has a parent, so inherit the parents properties
	if(Parent)
	{

		// covers top or bottom left
		if( Left )
		{
		
			if(Top)
			{
				//Top-left
				NewNode->Box.min.x = Parent->Box.min.x ;
				NewNode->Box.min.y = LowestHeight;
				NewNode->Box.min.z = Parent->Box.min.z;
				
				NewNode->Box.max.x = Parent->Box.max.x - ((Parent->Box.max.x-Parent->Box.min.x) / 2);
				NewNode->Box.max.y = HighestHeight;
				NewNode->Box.max.z = Parent->Box.max.z - ((Parent->Box.max.z-Parent->Box.min.z) / 2);
			}
			else
			{
				//bottom-left
				NewNode->Box.min.x = Parent->Box.min.x;
				NewNode->Box.min.y = LowestHeight;
				NewNode->Box.min.z = Parent->Box.min.z + ((Parent->Box.max.z-Parent->Box.min.z) / 2);
				
				NewNode->Box.max.x = Parent->Box.max.x  - ((Parent->Box.max.x-Parent->Box.min.x) / 2);
				NewNode->Box.max.y = HighestHeight;
				NewNode->Box.max.z = Parent->Box.max.z ;
			}
		}
		else // Covers top or bottom right
		{
			if(Top)
			{
				//Top-right
				NewNode->Box.min.x = Parent->Box.min.x + ((Parent->Box.max.x-Parent->Box.min.x) / 2);
				NewNode->Box.min.y = LowestHeight;
				NewNode->Box.min.z = Parent->Box.min.z;
				
				NewNode->Box.max.x = Parent->Box.max.x ;
				NewNode->Box.max.y = HighestHeight;
				NewNode->Box.max.z = Parent->Box.max.z - ((Parent->Box.max.z-Parent->Box.min.z) / 2);
			}
			else
			{
				//bottom-right
				NewNode->Box.min.x = Parent->Box.min.x + ((Parent->Box.max.x-Parent->Box.min.x) / 2);
				NewNode->Box.min.y = LowestHeight;
				NewNode->Box.min.z = Parent->Box.min.z + ((Parent->Box.max.z-Parent->Box.min.z) / 2);
				
				NewNode->Box.max.x = Parent->Box.max.x ;
				NewNode->Box.max.y = HighestHeight;
				NewNode->Box.max.z = Parent->Box.max.z ;
			}
		}
	}

	//no parent, must be the root node
	else
	{
		NewNode->Box.min.x = 0;
		NewNode->Box.min.y = LowestHeight;
		NewNode->Box.min.z = 0;
		NewNode->Box.max.x = (Real)Width * Spacing ;
		NewNode->Box.max.y = HighestHeight;
		NewNode->Box.max.z = (Real)Height * Spacing;
	}



	//If this is the maximum depth, add objects to it.
	if( CurrentDepth == MaxDepth )
	{

		//Create a vertex buffer for this particular spot
		CreateVertexbuffer( NewNode );
		//NewNode->GenerateTexture( *ScreenPtr, this, NULL , NULL, NULL, NULL );
	
		//Insert a reference to this node in the reference
		//array.
		int ix = (int) ((Real)NewNode->Box.min.x / ((Real)(NodeWidth-1)*Spacing));
		int iy = (int) ((Real)NewNode->Box.min.z / ((Real)(NodeHeight-1)*Spacing));

		NodeReferenceList[ix+iy*nAcross].Node = NewNode;

		NewNode->Patch = terrainMap->GetPatchAt( ix, iy );
		NewNode->InitGrass();

	}
	

	// Finally, start building the children if we aren't at the deepest level
	if(CurrentDepth<MaxDepth)
	{
		// Create new nodes
		NewNode->TopLeft  = &NodeList[CurrentIndex];//new QuadNode;
		NewNode->iTL      = CurrentIndex;
		CurrentIndex++;
		NewNode->TopRight = &NodeList[CurrentIndex];//new QuadNode;
		NewNode->iTR      = CurrentIndex;
		CurrentIndex++;
		NewNode->BotLeft  = &NodeList[CurrentIndex];//new QuadNode;
		NewNode->iBL      = CurrentIndex;
		CurrentIndex++;
		NewNode->BotRight = &NodeList[CurrentIndex];//new QuadNode;
		NewNode->iBR      = CurrentIndex;
		CurrentIndex++;
		
		//Build the nodes up
		BuildTree(NewNode, NewNode->TopLeft, true, true);
		BuildTree(NewNode, NewNode->TopRight, false, true);
		BuildTree(NewNode, NewNode->BotLeft, true, false);
		BuildTree(NewNode, NewNode->BotRight, false, false);
	}
	else
	{
		NewNode->TopLeft  = NULL;
		NewNode->TopRight = NULL;
		NewNode->BotLeft  = NULL;
		NewNode->BotRight = NULL;

		NewNode->iTL = -1;
		NewNode->iTR = -1;
		NewNode->iBL = -1;
		NewNode->iBR = -1;
	}


	CurrentDepth--;
}


bool Terrain::CheckTerrainCollision (Vector3D a, Vector3D b, Vector3D* oPos, Vector3D* oNormal)
{
	float h = this->GetHeightValue( Position( a ), NULL, oNormal );
	*oPos = Vector3D( a.x, h, a.z );

	return true;
}

//////// GetLayerVisibility ///////////////////
//
// Description:
//
// Returns the visibility of a layer at the
// given map point ( in world coordinates).
// Value is between 0.0 (blocked) to 1.0 (completly
// visible ).
//
///////////////////////////////////////////////

float Terrain::GetLayerVisibility( int layerIndex, float wx, float wz ) 
{
	//convert world coordinates to layer coordinates
	float visibility=0.0f;
	for ( int i = layerIndex; i<LayerList.size(); i++ )
	{
		TerrainLayer* layer = LayerList[i];
		float px = wx / (Width*Spacing);
		float pz = wz / (Height*Spacing);

		int mx = px*layer->Width();
		int mz = pz*layer->Height();

		if( i == layerIndex )
			visibility = layer->GetOpacity( mx, mz );
		else 
			visibility *= (1.0f-layer->GetOpacity( mx, mz ) ); 
	}

	return visibility;

}


float Terrain::GetLightValue( float wx, float wz )
{
	float px = wx / (Width *Spacing);
	float pz = wz / (Height*Spacing);

	if(px < 0 || pz < 0 || px >= 1.0f || pz >= 1.0f)
		return 0.0f;
	int mx = px * WMPtr->LM.Width ();
	int mz = pz * WMPtr->LM.Height();
	
	return WMPtr->LM.LightValue( mx, mz );
}


//////// GetHeightValue ///////////////////////
//
// Description:
//
// Returns the map height at a specified location.
// This is done at the polygon level, so the 
// height value will always exactly line up with
// the surface of the polygon.
//
// Basically, the function works like so:
// 1) Figures out which triangle we're over.
// 2) Figures out the plane of the triangle we're
//    over.
// 3) Creates a line that goes straight up and
//    down at the x/z coordinates
// 4) Finds where the line intersects our plane.
//    The point where it hits is out return value.
//
///////////////////////////////////////////////

Real Terrain::GetHeightValue( Position &ObjectLocation, Real* Slope, Vector3D* SlopeNormal, D3DXPLANE* TPlane, Vector3D* RoundSlopeNormal )
{
		//declarations
	D3DXPLANE TrianglePlane;
	int Triangle=0; // 0 == bottom left triangle, 1 = top right triangle
	Real ReturnValue = ObjectLocation.y;
	Vector3D a, b, c, na, nb, nc;

	Real wx = ObjectLocation.x;
	Real wy = ObjectLocation.z;

	if(wx < 0 || wy < 0 || wx >= Width*Spacing || wy >= Height*Spacing)
		return 0.0f;


	//get map coordinates
	int mx = (int) ((Real)wx/(Real)Spacing);
	int my = (int) ((Real)wy/(Real)Spacing);


	if(mx >= Width)
		mx = Width-1;

	if(my >= Height)
		my = Height-1;

	//figure out which triangle to test (bottom left or top right)..
	//this is sort of a confusing algorithm, but it basically is taking
	//advantage of the fact that the tile is a quad. If the sum of the percentage
	//across the tile for both directions is less than %50, than it's the bottom
	//left triangle. Try this out on paper to see what I mean.

	Real wxt = (wx/Spacing);
	Real wyt = (wy/Spacing);

	Real Balance = (wxt-mx) + (wyt-my);
	if( Balance < 1 )
		Triangle = 0;
	else
		Triangle = 1;


	//Figure out which heightmap we're testing (the water map or the land map)
	Real* TestMap = Map;
	
	//Figure out triangle locations
	D3DXVECTOR3 List[3];	

	//bottom left triangle
	if(Triangle == 0)
	{

		//get triangle vertices then calculate the plane from them. Note that
		//we're just recalculating them instead of locking the vertex buffer
		//and looking them up. That way we can make the vertex buffer write only
		//which is a lot faster. Plus, reading from video memory is slow, so
		//recalculating them is likely faster, anyway.
		List[0].x = (Real) mx*Spacing;
		List[0].y = TestMap[mx+my*Width];
		List[0].z = (Real) my*Spacing;

		List[1].x = (Real) mx*Spacing;
		List[1].y = TestMap[mx+((my+1)*Width)];
		List[1].z = (Real) (my+1)*Spacing;

		List[2].x = (Real) (mx+1)*Spacing;
		List[2].y = TestMap[(mx+1)+(my*Width)];
		List[2].z = (Real) my*Spacing;

		D3DXPlaneFromPoints(&TrianglePlane, &List[0], &List[1], &List[2]);


		if(TPlane)
			*TPlane = TrianglePlane;

		//Now find out where the player intersects the plane to find the
		//height.
		D3DXVECTOR3 L1, L2, Intersection;


		L1.x = wx;
		L1.y = 5000;
		L1.z = wy;

		L2.x = wx;
		L2.y = -5000;
		L2.z = wy;


		D3DXPlaneIntersectLine(&Intersection, &TrianglePlane, &L1, &L2);

		if(Slope)
		{
			D3DXVECTOR3 normal;
			normal.x = 0;
			normal.y = 1;
			normal.z = 0;

			*Slope = D3DXPlaneDotNormal( &TrianglePlane, &normal);
		}

		if(SlopeNormal)
		{
            SlopeNormal->x = TrianglePlane.a;
			SlopeNormal->y = TrianglePlane.b;
			SlopeNormal->z = TrianglePlane.c;
		}

		//return the y value
		ReturnValue = Intersection.y;

	}

	//top right triangle
	else if(Triangle == 1)
	{
		List[0].x = (Real) mx*Spacing;
		List[0].y = TestMap[mx+((my+1)*Width)];
		List[0].z = (Real) (my+1)*Spacing;

		List[1].x = (Real) (mx+1)*Spacing;
		List[1].y = TestMap[(mx+1)+((my+1)*Width)];
		List[1].z = (Real) (my+1)*Spacing;

		List[2].x = (Real) (mx+1)*Spacing;
		List[2].y = TestMap[(mx+1)+my*Width];
		List[2].z = (Real) my*Spacing;

		

		D3DXPlaneFromPoints(&TrianglePlane, &List[0], &List[1], &List[2]);
		if(TPlane)
			*TPlane = TrianglePlane;


		///find the intersection with the plane
		D3DXVECTOR3 L1, L2, Intersection;

		
		L1.x = wx;
		L1.y = 5000;
		L1.z = wy;

		L2.x = wx;
		L2.y = -5000;
		L2.z = wy;
		

		D3DXPlaneIntersectLine(&Intersection, &TrianglePlane, &L1, &L2);

		if(Slope)
		{
			D3DXVECTOR3 normal;
			normal.x = 0;
			normal.y = 1;
			normal.z = 0;

			*Slope = D3DXPlaneDotNormal( &TrianglePlane, &normal);
		}

		if(SlopeNormal)
		{
            SlopeNormal->x = TrianglePlane.a;
			SlopeNormal->y = TrianglePlane.b;
			SlopeNormal->z = TrianglePlane.c;		
		}
	
		//return the y value
		ReturnValue = Intersection.y;
	}


	return ReturnValue;
}



void _fastcall Terrain::DrawTree ( QuadNode* Node, bool ParentFullyVisible )
{
	int result=0;
	
	if(!ParentFullyVisible)
		result = ScreenPtr->CheckAABB( Node->Box );
	else
		result = 2;

	CurrentDepth ++;
	

	//box isn't even visible, cancel drawing
	if(result == 0)
	{
		result = ScreenPtr->CheckAABB( Node->Box );

		if(result == 0)
		{
			CurrentDepth--;
			return;
		}
		else
		{
			result = 3;
		}
	}

	//box is partially visible, either recurse to next level or draw if it's at the max
	//depth
	if(result>0)
	{
		if(CurrentDepth == MaxDepth)
		{
			if(result != 3 && !Node->Invisible  )
			{
				DrawQueue.push_back( Node );		
			}

			//ScreenPtr->SetTexture( -1, 1 );
			//ScreenPtr->SetTextureMode( TMNormal );
			//ScreenPtr->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA((int)WMPtr->WorldWeather.IntermediateState.AR, (int)WMPtr->WorldWeather.IntermediateState.AG, (int)WMPtr->WorldWeather.IntermediateState.AB, 255));
			//ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		
		}
		else
		{
			bool isVisible ;
			if( result == 2)
				isVisible = true;
			else
				isVisible = false;

			if(Node->TopLeft)
				DrawTree(Node->TopLeft, isVisible);
			if(Node->TopRight)
				DrawTree(Node->TopRight, isVisible);
			if(Node->BotLeft)
				DrawTree(Node->BotLeft, isVisible);
			if(Node->BotRight)
				DrawTree(Node->BotRight, isVisible);
				
		}
	}
	

	CurrentDepth --;

}
	
bool Terrain::InitTerrainShader()
{
	//Run the shader through the compiler
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pErrors;
	
	HRESULT HR = D3DXAssembleShaderFromFileA( "..\\scripts\\shaders\\layer.psh", NULL, NULL, NULL, &pCode, &pErrors );

	if ( FAILED(HR) )
	{
		OutputDebugString( "Failed to assemble the pixel shader, errors:\n" );
		OutputDebugString( (char*)pErrors->GetBufferPointer() );
		OutputDebugString( "\n" );
	}
	else
	{


		//Create the shadar!
		if(FAILED( ScreenPtr->D3DDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &TerrainShader ) ) )
		{
			//OH TEH NOS!
			pCode->Release();
			return false;
		}
		
		pCode->Release();
	}


	D3DVERTEXELEMENT9 shaderDeclaration[] = 
	{
		//stream 0
		{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	m_grassVertexShader.Init(ScreenPtr, "../scripts/shaders/grass.nvv", shaderDeclaration );


	return true; 

}

void Terrain::SetShaderValues() 
{
	D3DXVECTOR4 fog(WMPtr->WorldWeather.CurrentState.FogStart, 
					WMPtr->WorldWeather.CurrentState.FogEnd, 
					1/WMPtr->WorldWeather.CurrentState.ViewingDistance, 
					1.0f );
	ScreenPtr->SetVertexShaderConstant(8, &fog, 1);

	D3DXVECTOR4 lightcolor( WMPtr->WorldWeather.CurrentState.SR/255.0f,
							WMPtr->WorldWeather.CurrentState.SG/255.0f, 
							WMPtr->WorldWeather.CurrentState.SB/255.0f, 
							1.0f );
	D3DXVECTOR4 materialcolor( 1.0f, 1.0f, 1.0f, 1.0f );
	ScreenPtr->SetVertexShaderConstant(10, &lightcolor, 1);
	ScreenPtr->SetVertexShaderConstant(11, &materialcolor, 1);

	ScreenPtr->SetRenderState( D3DRS_LIGHTING, FALSE );
	ScreenPtr->SetRenderState( D3DRS_FOGENABLE, TRUE );
}

void Terrain::Update()
{
	terrainMap->Update();
}

void Terrain::UpdateLOD( Vector3D* camera )
{
	terrainMap->UpdateLOD( camera );
}

void Terrain::Draw()
{
	CurrentDepth = 0;
	ScreenPtr->SetVertexShader( this->terrainMap->Shader());

	DrawQueue.clear();
	DrawTree(Tree, false);


	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	WMPtr->LM.UnlightAll(*ScreenPtr);           

	//Draw the landscape

	// Set up a material. 
	Matrix Ident;
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = (Real)WMPtr->WorldWeather.IntermediateState.SR/255.0f; mtrl.Ambient.r = 0.0f;
	mtrl.Diffuse.g = (Real)WMPtr->WorldWeather.IntermediateState.SG/255.0f; mtrl.Ambient.g = 0.0f;
	mtrl.Diffuse.b = (Real)WMPtr->WorldWeather.IntermediateState.SB/255.0f; mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = 1; mtrl.Ambient.a = 1.0f;
	ScreenPtr->D3DDevice->SetMaterial( &mtrl );

	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);
	

	ScreenPtr->SetLinearTextureFiltering(1, 3);

	ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );



	ScreenPtr->D3DDevice->SetVertexShader( this->terrainMap->Shader());
	ScreenPtr->D3DDevice->SetVertexDeclaration( this->terrainMap->ShaderDeclaration());

	ScreenPtr->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ScreenPtr->D3DDevice->SetPixelShader( TerrainShader );

	
	SetShaderValues();

	for(int l=0;l<LayerList.size(); l++)
	{
		ScreenPtr->SetRenderState(D3DRS_DEPTHBIAS, l);
		ScreenPtr->SetTexture( LayerList[l]->ImageTexture, 0 );
		ScreenPtr->SetTexture( &LayerList[l]->AlphaMask, 1 );
		ScreenPtr->SetTexture( WMPtr->LM.GetTexture(), 2);

		// Generate spheremap texture coords, and shift them over
        D3DXMATRIX mat;
        mat._11 = 0.5f; mat._12 = 0.0f; mat._13 = 0.0f; mat._14 = 0.0f; 
        mat._21 = 0.0f; mat._22 =-0.5f; mat._23 = 0.0f; mat._24 = 0.0f; 
        mat._31 = 0.0f; mat._32 = 0.0f; mat._33 = 1.0f; mat._34 = 0.0f; 
        mat._41 = 0.5f; mat._42 = 0.5f; mat._43 = 0.0f; mat._44 = 1.0f; 
        ScreenPtr->D3DDevice->SetTransform( D3DTS_TEXTURE1, &mat );
        ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

		for( int i=0; i<DrawQueue.size(); i++)
		{
			QuadNode* Node = DrawQueue[i];	
			Node->Patch->Draw();
		}
	}
	

	ScreenPtr->SetRenderState(D3DRS_DEPTHBIAS, 0);
	ScreenPtr->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	ScreenPtr->D3DDevice->SetPixelShader( 0 );
					
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAREF, 32);
		
	//Reset the blend modes
	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ScreenPtr->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	ScreenPtr->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	ScreenPtr->SetMaterial( NULL );

	//Draw all the grass
	if(DrawGrass)
	{
		m_grassVertexShader.Activate();
	
		for( int i=0; i<DrawQueue.size(); i++)
		{
		
			QuadNode* Node = DrawQueue[i];	
			Vector3D nodePoint = Node->Center();
			Vector3D d = nodePoint- WMPtr->LocalPlayer.GetLocation().GetVector() ;

			if( d.Mag()< 500.0f+Node->DiagonalSize() )
				Node->m_grass.Draw();
		}
	}

	//ScreenPtr->SetRenderState( D3DRS_LIGHTING, T );
	//ScreenPtr->SetRenderState( D3DRS_FOGENABLE, FALSE );
}
