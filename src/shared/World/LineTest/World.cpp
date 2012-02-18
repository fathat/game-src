#include <windows.h>
#include "world.h"
#include "..\ini\ini.h"
#include "Soldier.h"
#include "NPC.h"
#include "waterfall.h"
#include "weaponofdeath.h"
#include "..\imageloader\image.h"

#define GetBit(bits, bitposition) (((1<<bitposition)&bits)>>bitposition)
#define SetBit(bits,bit, bitposition) (bits|=(bit<<bitposition))

 

//checks to see if this node is completly obscured by the
//"obscuring map", and if it is we just call it invisible and do nothing
//with it.
void QuadNode::CalculateVisibility( WorldManager* WM, WorldManager* ObscuringWM )
{
	//Declarations
	int x = (Box.AABBmin.x/WM->Spacing);
	int y = (Box.AABBmin.z/WM->Spacing);
	int mx, my;
	

	//Go through each map point this covers and make sure
	//none of the points poke their way over the obscuring
	//map.
	Invisible=true; //assume its true to start with
	for(int ly=0; ly<WM->NodeHeight; ly++)
	{
		for(int lx=0; lx<WM->NodeWidth; lx++)
		{
			//Figure out the absolute coordinates
			mx = x+lx;
			my = y+ly;

			//Check for the map poking over! O_O
			if( WM->Map[mx+my*WM->Width] >= ObscuringWM->Map[mx+my*ObscuringWM->Width] )
			{
				//well its not obscured, so we can't have it be invisible
				Invisible = false;
			}
		}
	}
}


void QuadNode::MakeDent(int vx, int vy, Real Amount, WorldManager* WM)
{
	//Declarations
	LandscapeVertex* List;
	int mapx = vx;
	int mapy = vy;

	int x = mapx-(Box.AABBmin.x/WM->Spacing);
	int y = mapy-(Box.AABBmin.z/WM->Spacing);


	MapVB->Lock((BYTE**)&List);
	
	//fill in vertex data
	List[x+y*(WM->NodeWidth-1)].position.y -= Amount;
	

	//Calculate the normal
	Vector3D A, B, R;

	A.x = (Real)WM->Spacing;
	A.y = WM->Map[(mapx+1)+mapy*WM->Width] - WM->Map[mapx+mapy*WM->Width];
	A.z = 0;

	
	B.x = 0;
	B.y = WM->Map[mapx+((mapy+1)*WM->Width)] - WM->Map[mapx+mapy*WM->Width];
	B.z = (Real)WM->Spacing;

	R = B.Cross(A);
	R.Normalize();

	List[x+y*(WM->NodeWidth-1)].normal.x = R.x;
	List[x+y*(WM->NodeWidth-1)].normal.y = R.y;
	List[x+y*(WM->NodeWidth-1)].normal.z = R.z;
	
	
	MapVB->Unlock();
}

void QuadNode::CalcFluidMatrix( WorldManager* WM )
{
	D3DXPLANE AveragePlane;

	D3DXVECTOR3 v1, v2, v3;

	v1.x = Box.AABBmin.x/WM->Spacing;
	v1.z = Box.AABBmin.z/WM->Spacing;
	v2.x = Box.AABBmin.x/WM->Spacing;
	v2.z = Box.AABBmax.z/WM->Spacing;
	v3.x = Box.AABBmax.x/WM->Spacing;
	v3.z = (Box.AABBmax.z/WM->Spacing)-((Box.AABBmax.z-Box.AABBmin.z)/WM->Spacing)/2;

	v1.y = WM->Map[(int)v1.x + (int)v1.z/(int)WM->Width];
	v2.y = WM->Map[(int)v2.x + (int)v2.z/(int)WM->Width];
	v3.y = WM->Map[(int)v3.x + (int)v3.z/(int)WM->Width];

	D3DXPlaneFromPoints( &AveragePlane, &v1, &v2, &v3);

	FluidMatrix.Translate( AveragePlane.a, AveragePlane.b, AveragePlane.c );

}


bool QuadNode::GenerateTexture ( Screen3D& Screen,WorldManager* WM,  TextureHandle Cliff, TextureHandle Ground, 
					  TextureHandle Beach, TextureHandle Underwater )
{

	//Get the bits for the base textures
	ARGB_PIXEL* CliffBits = WM->CliffTexture.GetBits();
	ARGB_PIXEL* GrassBits = WM->GrassTexture.GetBits();

	if(!CliffBits)
		return false;
	if(!GrassBits)
		return false;

	WM->QuadTexWidth  = 64;
	WM->QuadTexHeight = 64;



    
	//create the texture and write our data into it
	QuadTexture.Init( Screen, "QuadTexNotFile", WM->QuadTexWidth, WM->QuadTexHeight, D3DFMT_A8R8G8B8 );



	//Figure out how many "levels" (mip-maps) there are
	//in the texture
	LPDIRECT3DTEXTURE8 TextureSurface = QuadTexture.GetTextureSurface();
	D3DSURFACE_DESC SurfDesc;
	D3DLOCKED_RECT  LockedArea;
	BYTE* Surface;
	SHORT* ShortSurface;
	int DivFactor=1;
	int nLevels  = TextureSurface->GetLevelCount();
	int Alpha;
	int Color255=16;

	//loop through each level
	for(int l=0; l<nLevels; l++)
	{

		//Get the surface's attributes
		TextureSurface->GetLevelDesc(l, &SurfDesc);
		

		//lock down a pointer
		TextureSurface->LockRect(l, &LockedArea, NULL, NULL);
		Surface = (BYTE*)LockedArea.pBits;
		ShortSurface = (SHORT*)LockedArea.pBits;
		

		//loop through each pixel in the surface
		for(int y=0; y<SurfDesc.Height; y++)
		{
			for(int x=0; x<SurfDesc.Width; x++)
			{
				//Declarations
				Real alpha=0;
				Real Slope;
				Vector3D SlopeNormal, SlopeNormalRight, SlopeNormalDown;
				Position Location;

				//Convert our texture pixel into world space
				Real rtx = (Real)x/(Real)SurfDesc.Width;
				Real rty = (Real)y/(Real)SurfDesc.Height;
				rtx *= (Real)(WM->NodeWidth)* WM->Spacing;
				rty *= (Real)(WM->NodeHeight)* WM->Spacing;
				rtx += (Real)Box.AABBmin.x;
				rty += (Real)Box.AABBmin.z;
				
				//Find the SlopeNormal for this point of the terrain
				Location.x = rtx;
				Location.z = rty;
				Real Height = WM->GetHeightValue( Location, &Slope, &SlopeNormal );

				//Get our alpha value
				SlopeNormal.Normalize();
				alpha = fabs(SlopeNormal.y);



				//If the alpha value is sort of in-between rock
				//and grass, randomely choose between one or the
				//other with a weight'd random something something
				/*if( alpha > .3f && alpha < .95f )
				{
					int Bound = alpha * 100.0f;

					if( rand() % 100 > Bound  )
						alpha = 0;
					else
						alpha = SlopeNormal.y;
				
				}*/
				
				//At extreme for grass
				if ( alpha >= .70f )
				{
					alpha = 1;
				}
				
				//At extreme for rock
				else if ( alpha < .70f )
				{
					alpha = 0;
				}
								


				//Now we want to write the value to the texture
				if(SurfDesc.Format == D3DFMT_A8R8G8B8 || SurfDesc.Format == D3DFMT_X8R8G8B8)
				{						
					Surface[(x*4)+0+y*(SurfDesc.Width*4)] = 255;
					Surface[(x*4)+1+y*(SurfDesc.Width*4)] = 255;
					Surface[(x*4)+2+y*(SurfDesc.Width*4)] = 255;
					Surface[(x*4)+3+y*(SurfDesc.Width*4)] = (int)(1.0f-alpha)*255.0f;
				}
				else if( SurfDesc.Format == D3DFMT_A4R4G4B4 )
				{
					Alpha = (1.0f-alpha)*15.0f;
					Color255 = 16;

					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 0), 0);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 1), 1);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 2), 2);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 3), 3);

					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 0), 4);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 1), 5);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 2), 6);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 3), 7);
					
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 0), 8);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 1), 9);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 2),10);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Color255, 3),11);

					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 0),12);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 1),13);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 2),14);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 3),15);
				}
				
				
			}
		}

		DivFactor *=2;

		//unlock the surface
		TextureSurface->UnlockRect(l);
	}



	return true;
}

bool QuadNode::InitializeGeometry( WorldManager* WM)
{

	//First, build the set of vertices for the geometry and
	//calculate things like bounding boxes
	LandscapeVertex* QuadVertices;
	QuadVertices = new LandscapeVertex[WM->NodeWidth*WM->NodeHeight];


	int mapx;
	int mapy;
	Real MinY= 10000;
	Real MaxY=-10000;


	//Go through every point and fill it in
	for(int y=0; y<WM->NodeHeight; y++)
	{
		for(int x=0; x<WM->NodeWidth; x++)
		{
			mapx = x+(Box.AABBmin.x/WM->Spacing);
			mapy = y+(Box.AABBmin.z/WM->Spacing);
			
			if(mapx+1 >= WM->Width ) mapx = WM->Width -2;
			if(mapy+1 >= WM->Height) mapy = WM->Height-2;

			//fill in vertex data
			QuadVertices[x+y*WM->NodeWidth].position.x = (mapx*WM->Spacing);
			QuadVertices[x+y*WM->NodeWidth].position.y = WM->Map[(mapx+(mapy*WM->Width))];
			QuadVertices[x+y*WM->NodeWidth].position.z = (mapy*WM->Spacing);
			QuadVertices[x+y*WM->NodeWidth].normal.y   = 1;
			QuadVertices[x+y*WM->NodeWidth].tu        = (Real) x/((Real)WM->NodeWidth/2);
			QuadVertices[x+y*WM->NodeWidth].tv        = (Real) y/((Real)WM->NodeWidth/2);
			QuadVertices[x+y*WM->NodeWidth].tu2         = (Real) x/((Real)WM->NodeWidth);
			QuadVertices[x+y*WM->NodeWidth].tv2         = (Real) (Real)((Real)y/((Real)WM->NodeHeight));

			if(WM->Map[(mapx+(mapy*WM->Width))]>MaxY)
				MaxY = WM->Map[(mapx+(mapy*WM->Width))];
			if(WM->Map[(mapx+(mapy*WM->Width))]<MinY)
				MinY = WM->Map[(mapx+(mapy*WM->Width))];


			//Calculate the normal
			Vector3D A, B, R;

			A.x = (Real) WM->Spacing;
			A.y = WM->Map[(mapx+1)+mapy*WM->Width] - WM->Map[mapx+mapy*WM->Width];
			A.z = 0;
			B.x = 0;
			B.y = WM->Map[mapx+((mapy+1)*WM->Width)] - WM->Map[mapx+mapy*WM->Width];
			B.z = (Real) WM->Spacing;

			//Normal is a cross between A and B
			R = B.Cross(A);
			R.Normalize();
			QuadVertices[x+y*WM->NodeWidth].normal.x = R.x;
			QuadVertices[x+y*WM->NodeWidth].normal.y = R.y;
			QuadVertices[x+y*WM->NodeWidth].normal.z = R.z;
			
			Vector3D R2;
            //Figure out the normal for the second triangle
			if(y<WM->NodeHeight-1 && x<WM->NodeWidth-1)
			{
				A.x = 0;
				A.y =  WM->Map[(mapx+1)+(mapy+1)*WM->Width] - WM->Map[mapx+(mapy+1)*WM->Width];
				A.z = (Real) -WM->Spacing;
				B.x = (Real) -WM->Spacing;
				B.y = WM->Map[(mapx+1)+(mapy+1)*WM->Width] - WM->Map[(mapx+1)+mapy*WM->Width];
				B.z = 0;

				R2 = B.Cross(A);			
				R2.Normalize();
			}

			

			//Classify the vertex in either the cliff list or the
			//grass list.
			if( R.y > .70f )
			{
				
				GrassVertices.push_back( x+y*WM->NodeWidth) ;

				//Add the indices into the index buffer
				if(y<WM->NodeHeight-1 && x<WM->NodeWidth-1)
				{
					GrassIndices.push_back( x+y*WM->NodeWidth );
					GrassIndices.push_back( x+((y+1)*WM->NodeWidth));
					GrassIndices.push_back( (x+1)+y*WM->NodeWidth);

					
					if(fabs(R2.y) < .70f  )
					{
						CliffIndices.push_back( x+(y+1)*WM->NodeWidth);
						CliffIndices.push_back( (x+1)+((y+1)*WM->NodeWidth));
						CliffIndices.push_back( (x+1)+y*WM->NodeWidth);
					}
					else
					{
					
						GrassIndices.push_back( x+(y+1)*WM->NodeWidth);
						GrassIndices.push_back( (x+1)+((y+1)*WM->NodeWidth));
						GrassIndices.push_back( (x+1)+y*WM->NodeWidth);
					
					}
					
				}
			}
			else
			{
				
				CliffVertices.push_back( x+y*WM->NodeWidth) ;
				
				//Add the indices into the index buffer
				if(y<WM->NodeHeight-1 && x<WM->NodeWidth-1)
				{
					
					CliffIndices.push_back( x+y*WM->NodeWidth );
					CliffIndices.push_back( x+((y+1)*WM->NodeWidth));
					CliffIndices.push_back( (x+1)+y*WM->NodeWidth);

					if(fabs(R2.y) <.70f  )
					{
						CliffIndices.push_back( x+(y+1)*WM->NodeWidth);
						CliffIndices.push_back( (x+1)+((y+1)*WM->NodeWidth));
						CliffIndices.push_back( (x+1)+y*WM->NodeWidth);
					}
					else
					{
						
						GrassIndices.push_back( x+(y+1)*WM->NodeWidth);
						GrassIndices.push_back( (x+1)+((y+1)*WM->NodeWidth));
						GrassIndices.push_back( (x+1)+y*WM->NodeWidth);
					}
				}
			}
		
		}
	}

	//Set the extents of the bounding box
	Box.AABBmax.y = MaxY;
	Box.AABBmin.y = MinY;

	//Now that we have indices and such we need to make the index buffers
	

	
	//Init Cliff IB
	WORD* IndexList;
	CliffData.IB.Init ( *WM->ScreenPtr, CliffIndices.size() );
	CliffData.IB.Lock(&IndexList);
	for(int i=0; i<CliffIndices.size(); i++)
	{
		IndexList[i] = CliffIndices[i];
	}
	CliffData.IB.Unlock();


	//Init grass IB
	GrassData.IB.Init ( *WM->ScreenPtr, GrassIndices.size() );
	GrassData.IB.Lock(&IndexList);
	for(int i=0; i<GrassIndices.size(); i++)
	{
		IndexList[i] = GrassIndices[i];
	}
	GrassData.IB.Unlock();
	
	

	//Hooray! The game hasn't crashed yet!
	return true;

}


void WorldManager::CreateContourMap()
{
	//Create the buffer for storing this stuff
	if(TexBuffer)
		delete []TexBuffer;
	TexBuffer = new int[Width*Height];

	//Now do blending something or other in order to do some 
	//such thing in hopes of something something. So yeah.
	//Calculate the normal
	Vector3D A, B, R;

	for(int y=1; y<Height-1; y++)
	{
		for(int x=1; x<Width-1; x++)
		{
		

			Real AverageDistance=0;
			AverageDistance += fabs(Map[(x+1)+y*Width] - Map[x+y*Width]);
			AverageDistance += fabs(Map[(x-1)+y*Width] - Map[x+y*Width]);
			AverageDistance += fabs(Map[x+(y+1)*Width] - Map[x+y*Width]);
			AverageDistance += fabs(Map[x+(y-1)*Width] - Map[x+y*Width]);
			AverageDistance /= 4;
			
			if ( AverageDistance > 8 )
				TexBuffer[x+y*Width] = 0;
			else
				TexBuffer[x+y*Width] = 255;


				
			
		}
	}


}


void WorldManager::SetMusic ( char * MusicFile )
{

}

void WorldManager::Init( Screen3D& Screen, char* MapDesc, MeshManager* MM,  SoundManager& AM,  GameTimer* Timer, NetworkManager* NM,  WorldManager* Parent )
{
	//Init local pointers to global services
	ScreenPtr = &Screen;
	MMPtr = MM;
	GTPtr = Timer;
	AMPtr = &AM;
	NMPtr = NM;
	ParentWM = Parent;

	InMovie = false;
	WinConditionsMet = false;

	//Initialize tree recursion variables
	CurrentDepth = 0;
	MaxDepth = 1;



	ZeroMemory( DynamicObjectList, MAX_OBJECTS*4);
	ZeroMemory( ObjectList, MAX_OBJECTS*4);
	ZeroMemory( ParticleBuffer, MAX_DRAWABLE_PARTICLES*4 );

	//Load in the map
	LoadMap( MapDesc );


	//Build up the tree
	nNodes = (int)((pow(4,(MaxDepth+1))-1)/3);
	NodeList = new QuadNode[nNodes];
	CurrentIndex = 0;

	

	Tree = new QuadNode;
	BuildTree(NULL, Tree, false, false);

	Water.Init( "..\\art\\areas\\water\\water.bmp", Screen, this );
	
	

}

void WorldManager::KickupDust( Real WorldX, Real WorldZ, Real Radius )
{
	Position DustPosition;
	DustPosition.x = WorldX;
	DustPosition.z = WorldZ;
	Real Height = this->GetHeightValue( DustPosition );

	DustParticles.MoveSpawn( WorldX, Height, WorldZ);
	DustParticles.SpawnParticles( Radius, false );
	
}


bool WorldManager::LoadHeightmap( char* bitmap )
{

	CGenImage HeightImage;

	HeightImage.Load(bitmap);

	Width  = HeightImage.GetWidth ();
	Height = HeightImage.GetHeight();

	ARGB_PIXEL* Bits = HeightImage.GetBits();

	if(Map)
		delete Map;

	Map = new Real[Width*Height];

	//loop through each pixel in the surface
	for(int y=0; y<Height; y++)
	{
		for(int x=0; x<Width; x++)
		{
			//grab the colors from the gdi surface
			BYTE HeightValue = Bits[x+y*Width].b;

			Map[x+(Height-1-y)*Width] = (HeightValue*20) / HEIGHT_SCALE ;
					
		}
	}

	return true;
}


void WorldManager::RestartLevel()
{
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(ObjectList[i])
		{
			delete ObjectList[i];
			ObjectList[i] = NULL;
		}

		if(DynamicObjectList[i])
		{
			delete DynamicObjectList[i];
			DynamicObjectList[i] = NULL;
		}
	}


	LocalPlayer.Health = 1000;
	LocalPlayer.EquipmentList.clear();
	LoadObjects( ObjectMap );
}


bool WorldManager::LoadObjects( char* MapFile )
{
    	//Open the file

	strcpy(ObjectMap, MapFile);

	if(MapFile)
	{
		if(strcmpi( MapFile, "") == 0)
			return false;
	}
	else
		return false; 
	FILE* File = fopen( MapFile, "rb" );
	
	if(File == NULL)
		return false;

	//Read the header data

	int MapID ;
	fread(&MapID, sizeof(int), 1, File);

	//make sure the map type is correct, if it's not then
	//return an error.
	if(MapID != ('M' + ('4' << 8) + ('D' << 16) + ('O' << 24)))
	{
		return false;
	}
	
	//Read the rest of the data
	int nObjectTypes;
	fread(&nObjectTypes, sizeof(int), 1, File);

	int nObjects ;
	fread(&nObjects, sizeof(int), 1, File);



	//read a list of all the objects types used
    char ObjectName[128];
	std::string strName;
	for( int i=0; i<nObjectTypes; i++)
	{
		fread(ObjectName, 128, 1, File);
		strName = ObjectName;
		ObjectTypeList.push_back(strName);
	}

	
	//read the object list
	char Buffer[128];
	for( i=0; i<nObjects; i++)
	{
		Object LoadObject;
		fread(&LoadObject, sizeof(Object), 1, File);

		
		
		GetIniValue( (char*)ObjectTypeList[LoadObject.Type].c_str(), "properties", "type", Buffer);






		if( strcmpi(Buffer, "soldier") == 0 )
		{
			Soldier* sceneobject = new Soldier;

			sceneobject->Init(*ScreenPtr, this, *MMPtr, *AMPtr,  (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			sceneobject->Location.x = LoadObject.x * (Width) * Spacing ;
			sceneobject->Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			sceneobject->Location.y = GetHeightValue( sceneobject->Location );
			sceneobject->Location.Yaw   = RADIAN(LoadObject.yaw);
			sceneobject->ObjectType = OT_DYNAMIC;
			sceneobject->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string


			sceneobject->AdjustBoundingBox();

			
			AddDynamic( sceneobject);
			
		}
		
		else if( strcmpi(Buffer, "npc") == 0 )
		{
			NPC* sceneobject = new NPC;

			sceneobject->Init(*ScreenPtr, this, *MMPtr, *AMPtr,  (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			sceneobject->Location.x = LoadObject.x * (Width) * Spacing ;
			sceneobject->Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			sceneobject->Location.y = GetHeightValue( sceneobject->Location );
			sceneobject->Location.Yaw   = RADIAN(LoadObject.yaw);
			sceneobject->ObjectType = OT_DYNAMIC;
			sceneobject->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string


			sceneobject->AdjustBoundingBox();

			
			AddDynamic( sceneobject);
			
		}

		else if( strcmpi(Buffer, "weaponofdeath") == 0 )
		{
			WeaponOfDeath* sceneobject = new WeaponOfDeath;

			sceneobject->Init(*ScreenPtr, this, *MMPtr, (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			sceneobject->Location.x = LoadObject.x * (Width) * Spacing ;
			sceneobject->Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			sceneobject->Location.y = GetHeightValue( sceneobject->Location );
			sceneobject->Location.Yaw   = RADIAN(LoadObject.yaw);
			sceneobject->ObjectType = OT_STATIC;
			sceneobject->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string


			sceneobject->AdjustBoundingBox();

			
			AddDynamic( sceneobject);
			
		}

		else if (strcmpi( Buffer, "avatar" ) == 0 && UseAvatar)
		{
			LocalPlayer.Init(*ScreenPtr, this, *MMPtr, *AMPtr,(char*)ObjectTypeList[LoadObject.Type].c_str() );
			LocalPlayer.Location.x = LoadObject.x * (Width) * Spacing ;
			LocalPlayer.Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			LocalPlayer.Location.y = GetHeightValue( LocalPlayer.Location );
			LocalPlayer.Location.Yaw   = RADIAN(LoadObject.yaw);
			LocalPlayer.ObjectType = OT_DYNAMIC;
		}
		else if (strcmpi( Buffer, "waterfall") == 0)
		{
			WaterFall* waterfall = new WaterFall;

			waterfall->Init(*ScreenPtr, this, *MMPtr, *AMPtr, (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			waterfall->Location.x = LoadObject.x * (Width) * Spacing ;
			waterfall->Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			waterfall->Location.y = GetHeightValue( waterfall->Location );
			waterfall->Location.Yaw   = RADIAN(LoadObject.yaw);
			waterfall->ObjectType = OT_DYNAMIC;
			waterfall->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string


			waterfall->AdjustBoundingBox();

			
			AddDynamic(waterfall);	

			

		}
		else if( strcmpi(Buffer, "weapon") == 0 )
		{
			Weapon* sceneobject = new Weapon;

			sceneobject->Init(*ScreenPtr, this, *MMPtr, *AMPtr,  (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			Position Location;

			Location.x = LoadObject.x * (Width) * Spacing ;		
			Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			Location.y = GetHeightValue( Location );
			Location.Yaw   = RADIAN(LoadObject.yaw);

			sceneobject->SetLocation( Location );
			sceneobject->Parent = NULL;
			sceneobject->ObjectType = OT_POWERUP;
			sceneobject->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string

			sceneobject->AdjustBoundingBox();
			
			AddDynamic( sceneobject);
			
		}

		else
		{
			SceneObject* sceneobject = new SceneObject;

			sceneobject->Init(*ScreenPtr, this, *MMPtr, *AMPtr, (char*)ObjectTypeList[LoadObject.Type].c_str() );
			
			sceneobject->Location.x = LoadObject.x * (Width) * Spacing ;
			sceneobject->Location.z = (Height - LoadObject.y * (Height)) * Spacing ;
			sceneobject->Location.y = GetHeightValue( sceneobject->Location );
			sceneobject->Location.Yaw   = RADIAN(LoadObject.yaw);
			sceneobject->ObjectType = OT_STATIC;
			sceneobject->Name       = LoadObject.name; //Yes, I can do this, because
													   //sceneobject->Name is an STL string
			
			
			sceneobject->AdjustBoundingBox();
			
			if(sceneobject->Stream)
				AddDynamic( sceneobject);
			else
				AddDynamic(sceneobject);	
		}


		

		
	}


	//close the file
	fclose( File);
	
	return true;

}

bool WorldManager::LoadBaseTextures( char* MapDesc )
{
	//Declarations
	char TextBuffer[128];
	char TextBuffer2[128];
	bool ReturnValue = true;

	

	//Load.. TEH TEXTURES!11
	GetIniValue(MapDesc, "map", "cliff", TextBuffer);
	GetIniValue(MapDesc, "map", "cliff_a", TextBuffer2);
	FILE* TestFile = fopen( TextBuffer, "r+b");
	if(TestFile)
	{
		fclose(TestFile);

		if(FAILED(CliffTexture.Load(TextBuffer)))
			ReturnValue = false;
	}
	if(strcmpi( TextBuffer2, "nostring" ) == 0 )
		hCliffTexture = ScreenPtr->GetTextureHandle( TextBuffer );
	else
		hCliffTexture = ScreenPtr->GetTextureHandle( TextBuffer, TextBuffer2 );

	GetIniValue(MapDesc, "map", "grass", TextBuffer);
	GetIniValue(MapDesc, "map", "grass_a", TextBuffer2);
	TestFile = fopen( TextBuffer, "r+b");
	if(TestFile)
	{
		fclose(TestFile);
		if(FAILED(GrassTexture.Load(TextBuffer)))
			ReturnValue = false;
	}
	if(strcmpi( TextBuffer2, "nostring" ) == 0 )
		hGrassTexture = ScreenPtr->GetTextureHandle( TextBuffer );
	else
		hGrassTexture = ScreenPtr->GetTextureHandle( TextBuffer, TextBuffer2 );
	

	//O_O!
	return ReturnValue;

}

bool WorldManager::LoadMap( char* MapDesc )
{

	//declarations
	char TempBuffer[128];

	LoadBaseTextures(MapDesc);

	//Load in the attributes first
	Width   = 128; //doesn't really matter because LoadHeightmap will set
	Height  = 128; //it anyway, unless it fails.
	Spacing = GetIniValue(MapDesc, "map", "spacing");
	MaxDepth = GetIniValue(MapDesc, "map", "quad_depth");
	bool notsolid = GetIniValue(MapDesc, "map", "notsolid");
	
	if(notsolid)
		SolidGround = false;
	else
		SolidGround  = true;

	int blendmode = GetIniValue(MapDesc, "map", "blend");

	if(blendmode == 0 || blendmode == 2)
	{
	
        SetLandscapeBlend(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	}
	else
	{
		SetLandscapeBlend(D3DBLEND_ONE, D3DBLEND_ONE );
	}



	//Next, load in the heightmap
	GetIniValue(MapDesc, "map", "heightmap", TempBuffer);
	if(!LoadHeightmap( TempBuffer))
		return false;

	//Create the contour map
	CreateContourMap();
	
	//load the dust particles
	GetIniValue(MapDesc, "particles", "ground_impact", TempBuffer);
	DustParticles.Init( *ScreenPtr, TempBuffer, this);

	
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

	nAcross = Width/NodeWidth;
	nDown   = Height/NodeHeight;

	NodeReferenceList = new QuadNodePtr[nAcross*nDown];


	NodeWidth++;
	NodeHeight++;

	//load in objects
	GetIniValue(MapDesc, "map", "objectmap", TempBuffer);
	if(!LoadObjects( TempBuffer) && UseAvatar )
	{
		LocalPlayer.Init(*ScreenPtr, this, *MMPtr, *AMPtr,"..\\art\\characters\\avatar.ini" );
		LocalPlayer.Location.x = (Width * Spacing) /2;
		LocalPlayer.Location.z = (Height * Spacing) /2 ;
		LocalPlayer.Location.y = GetHeightValue( LocalPlayer.Location );
		LocalPlayer.Location.Yaw   =0;
	}

	//Now, load in the colormap and detail map
	GetIniValue(MapDesc, "map", "colormap", TempBuffer);
	ColorMap = ScreenPtr->GetTextureHandle( TempBuffer );

	GetIniValue(MapDesc, "map", "detailmap", TempBuffer);
	DetailMap = ScreenPtr->GetTextureHandle( TempBuffer );
	
	GetIniValue(MapDesc, "map", "signeddetailmap", TempBuffer);
	DetailMapSigned = ScreenPtr->GetTextureHandle( TempBuffer );
    
	ObjectDetailMap = ScreenPtr->GetTextureHandle ( "..\\art\\maps\\detail-map.bmp");

	//Load in the weather
	Position StartLocation;
	GetIniValue(MapDesc, "map", "weather", TempBuffer);
	
	if(DrawSky)
		WorldWeather.Init(*ScreenPtr, TempBuffer, StartLocation , this);


	//construct an index buffer to use with quad nodes
	WORD* IndexList;

	IB.Init(*ScreenPtr, ((NodeWidth-1)*(NodeHeight-1)*6));
	IB.Lock(&IndexList);

	int IndexOffset=0;

 

	for(int y=0; y<NodeHeight-1; y++)
	{
		for(int x=0; x<NodeWidth-1; x++)
		{
			//add indices to the index buffer	
			IndexList[IndexOffset] = x+y*NodeWidth;
			IndexList[IndexOffset+1] = x+((y+1)*NodeWidth);
			IndexList[IndexOffset+2] = (x+1)+y*NodeWidth;
			IndexList[IndexOffset+3] = x+(y+1)*NodeWidth;
			IndexList[IndexOffset+4] = (x+1)+((y+1)*NodeWidth);
			IndexList[IndexOffset+5] = (x+1)+y*NodeWidth;
			IndexOffset += 6;
				
		}
	}

	IB.Unlock();

	//Load the crazy water
	if(WaterWM)
		delete WaterWM;
	WaterWM = NULL;

	GetIniValue(MapDesc, "map", "watermap", TempBuffer);
	if(strcmpi( TempBuffer, "nostring" ) != 0)
	{
		WaterWM = new WorldManager;
		WaterWM->UseAvatar = false; //No avatar for this map
		WaterWM->FluidMap  = true;  //set it as a map for water
		WaterWM->DrawSky   = false; //no weather/sky things
	
		WaterWM->Init( *ScreenPtr, TempBuffer, MMPtr, *AMPtr, GTPtr, NMPtr, this);
	}


	return true;
}


void WorldManager::CreateVertexbuffer( QuadNode* Node)
{
	LandscapeVertex* List;

	
	Node->MapVB = new VertexBuffer;

	Node->MapVB->Init( *ScreenPtr, D3DFVF_LANDSCAPEVERTEX, sizeof( LandscapeVertex ), NodeWidth * NodeHeight );
	Node->MapVB->Lock((BYTE**)&List);

	int mapx;
	int mapy;


	Real MinY= 10000;
	Real MaxY=-10000;


	for(int y=0; y<NodeHeight; y++)
	{
		for(int x=0; x<NodeWidth; x++)
		{
			mapx = x+(Node->Box.AABBmin.x/Spacing);
			mapy = y+(Node->Box.AABBmin.z/Spacing);
			
			if(mapx+1 >= Width ) mapx = Width -2;
			if(mapy+1 >= Height) mapy = Height-2;

			//fill in vertex data
			List[x+y*NodeWidth].position.x = (mapx*Spacing);
			List[x+y*NodeWidth].position.y = Map[(mapx+(mapy*Width))];
			List[x+y*NodeWidth].position.z = (mapy*Spacing);
			List[x+y*NodeWidth].normal.y   = 1;
			List[x+y*NodeWidth].tu        = (Real) x/((Real)NodeWidth/2);
			List[x+y*NodeWidth].tv        = (Real) y/((Real)NodeWidth/2);
			List[x+y*NodeWidth].tu2         = (Real) x*2;
			List[x+y*NodeWidth].tv2         = (Real) y*2;

			if(Map[(mapx+(mapy*Width))]>MaxY)
				MaxY = Map[(mapx+(mapy*Width))];
			if(Map[(mapx+(mapy*Width))]<MinY)
				MinY = Map[(mapx+(mapy*Width))];


			//Calculate the normal
			Vector3D A, B, R;

			A.x = (Real) Spacing;
			A.y = Map[(mapx+1)+mapy*Width] - Map[mapx+mapy*Width];
			A.z = 0;

			
			B.x = 0;
			B.y = Map[mapx+((mapy+1)*Width)] - Map[mapx+mapy*Width];
			B.z = (Real) Spacing;

			R = B.Cross(A);
			R.Normalize();

			List[x+y*NodeWidth].normal.x = R.x;
			List[x+y*NodeWidth].normal.y = R.y;
			List[x+y*NodeWidth].normal.z = R.z;
			
		}
	}

	Node->Box.AABBmax.y = MaxY;
	Node->Box.AABBmin.y = MinY;
	

	Node->MapVB->Unlock();

	Node->InitializeGeometry( this );

}


void WorldManager::MakeDent( int vx, int vy, Real Amount)
{
	Map[vx+vy*Width] -= Amount;

	Real tx=vx, ty=vy;
	tx /= NodeWidth-1;
	ty /= NodeHeight-1;

	//NodeReferenceList[(int)tx+(int)(ty*nAcross)].Node->MakeDent((int)vx, (int)vy, Amount, this);

	//for(int i=0; i<nAcross*nDown; i++)
	CreateVertexbuffer(NodeReferenceList[(int)tx+(int)ty*nAcross].Node);
	QuadNode* NodePointer = NodeReferenceList[(int)tx+(int)ty*nAcross].Node;

	if(vx -(NodePointer->Box.AABBmin.x/Spacing) < 1 && tx >= 1)
	{
		CreateVertexbuffer(NodeReferenceList[(int)(tx-1)+(int)ty*nAcross].Node);
	}

	if(vy -(NodePointer->Box.AABBmin.z/Spacing) < 1 && ty >= 1)
	{
		CreateVertexbuffer(NodeReferenceList[(int)tx+(int)(ty-1)*nAcross].Node);
	}

	if(vx -(NodePointer->Box.AABBmin.x/Spacing) >= NodeWidth-2 && tx < nAcross-1)
	{
		CreateVertexbuffer(NodeReferenceList[(int)(tx+1)+(int)ty*nAcross].Node);
	}

	if(vy -(NodePointer->Box.AABBmin.z/Spacing) >= NodeHeight-2 && ty < nDown-1)
	{
		CreateVertexbuffer(NodeReferenceList[(int)(tx)+(int)(ty+1)*nAcross].Node);
	}


}


void WorldManager::MakeBlast( Real WorldX, Real WorldY, Real Radius )
{

}

SceneHandle WorldManager::Add ( SceneObject* object)
{
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(ObjectList[i] == NULL)
		{
			ObjectList[i] = object;

			
			nObjects++;

			return i;
		}
	}

	

	return -1;
}

void WorldManager::Cleanup()
{

	if(TexBuffer)
		delete TexBuffer;
	if(Map)
		delete Map;
	if(NodeReferenceList)
		delete NodeReferenceList;
	if(Tree)
		delete Tree;
	if(WaterWM)
		delete WaterWM;


	Map				  = NULL;
	NodeReferenceList = NULL;
	WaterWM			  = NULL;
	Tree			  = NULL;
	TexBuffer         = NULL;
	
	
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(ObjectList[i])
		{
			delete ObjectList[i];
			ObjectList[i] = NULL;
		}

		if(DynamicObjectList[i])
		{
			delete DynamicObjectList[i];
			DynamicObjectList[i] = NULL;
		}
	}
}

void WorldManager::Kill ( SceneHandle Who )
{
	if(ObjectList[Who])
	{
		delete ObjectList[Who];
		ObjectList[Who] = NULL;
		nObjects--;
	}
	
}

void WorldManager::Update( Real FrameTime )
{
	InMovie = false;
	TopMovieText = "";
	BottomMovieText = "";

    for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(ObjectList[i])
		{
			ObjectList[i]->Update( FrameTime, this);
		}

		if(DynamicObjectList[i])
			DynamicObjectList[i]->Update(FrameTime, this);

		
	}

	for(int j=0; j<NetworkObjectList.size(); j++)
	{
		if(NetworkObjectList[j])
			NetworkObjectList[j]->Update(FrameTime, this);
	}

	if(Viewer.Pos && DrawSky)
		WorldWeather.Update(FrameTime, *Viewer.Pos);

	if(UseAvatar)
		LocalPlayer.Update( FrameTime, this );

	DustParticles.Update( this->GTPtr->GetFrameTime());


	if(WaterWM)
		WaterWM->Update( FrameTime );

	
}


//Particle functions
void WorldManager::AddStream( ParticleStream* stream )
{
	//loop through and find an empty spot to place it
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
	{
		//blank spot?
		if(ParticleBuffer[i] == NULL)
		{
			//assign the texture a spot
			ParticleBuffer[i] = stream;
			
			//done, return
			return;
		}

	}
}

void WorldManager::ClearStreamList( )
{

	//loop through and kill things
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
		ParticleBuffer[i] = NULL;
	
}



void WorldManager::Draw( Screen3D& Screen )
{

	// Set the transforms back to default
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    Screen.D3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
	Screen.D3DDevice->SetTransform( D3DTS_VIEW, &matWorld );


	
	// Set some variables for the view bobbing
	if(UseAvatar)
	{
		Viewer.HeightDisplacement = LocalPlayer.ViewBob;
		Viewer.MaxHeightDisplacement = LocalPlayer.MaxViewBob;
	}

	if(DrawSky)
		WorldWeather.Draw(Screen,Viewer);


	// Get the current lookat matrix
	Matrix LookAtMatrix = Viewer.GetLookAtMatrix( Viewer.Zoom );
	Screen.SetViewTransform( LookAtMatrix, true );

	if(DrawSky)
		LM.LightScene( Screen );


	//Set the overhead view if we're doing a check on the frustrum
	/*if(DebugFieldOfView)
	{
		Viewer.CS = CS_FOLLOW;
		Viewer.Pitch = RADIAN(45);
		LookAtMatrix = Viewer.GetLookAtMatrix( 1200 );
		Screen.SetViewTransform( LookAtMatrix, false );
		Viewer.Pitch = 0;
	}*/
	


	Screen.D3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(120, 100, 100));

	ObjectsDrawn   = 0;
	ObjectsTested  = 0;
	RecursionCount = 0;
	CurrentDepth   = 0;


	//Set the landscape drawing render states
	Matrix M;
	ScreenPtr->SetTransform(M);
	ScreenPtr->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(10, 10, 10, 255));
	ScreenPtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	ScreenPtr->SetRenderState(D3DRS_LIGHTING, TRUE);
 	ScreenPtr->SetTextureMode( TMDarkMap);


	//Set the appropriate textures
	ScreenPtr->SetTexture( ColorMap, 1 );
	ScreenPtr->SetTexture( DetailMap, 0 ); 

	//Draw the landscape and all the objects
	DrawTree(Tree, false);
	DrawDynamicObjects( Screen);
	DrawNetworkObjects( Screen);

	//AddStream( &DustParticles );
	DustParticles.Draw( Screen, this->Viewer);

	//Draw the player if it's in 3rd person view
	if( UseAvatar)
	{
		LocalPlayer.Draw(Screen, *MMPtr, Viewer, *GTPtr);
	}

	
	//Return the textures to the previous state
	ScreenPtr->SetTextureMode( TMNormal );
	ScreenPtr->SetTexture( -1, 1 );


	//loop through and find things in the buffer to draw
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
	{
		//filled spot?
		if(ParticleBuffer[i])
		{
			//draw the crazy faceman!
			ParticleBuffer[i]->Draw(Screen, this->Viewer);
		}

	}
	
	ClearStreamList();

    //wo0t, done
	Screen.D3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(20, 20, 20));


	if(WaterWM)
	{
		WaterWM->Viewer = Viewer;
		Screen.SetRenderState( D3DRS_ZBIAS, 2 );
		WaterWM->Draw( *ScreenPtr );
		Screen.SetRenderState( D3DRS_ZBIAS, 0 );
	}



 

}


void WorldManager::BuildTree( QuadNode* Parent, QuadNode* NewNode, bool Left, bool Top )
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
				NewNode->Box.AABBmin.x = Parent->Box.AABBmin.x ;
				NewNode->Box.AABBmin.y = -1000;
				NewNode->Box.AABBmin.z = Parent->Box.AABBmin.z;
				
				NewNode->Box.AABBmax.x = Parent->Box.AABBmax.x - ((Parent->Box.AABBmax.x-Parent->Box.AABBmin.x) / 2);
				NewNode->Box.AABBmax.y = 1000;
				NewNode->Box.AABBmax.z = Parent->Box.AABBmax.z - ((Parent->Box.AABBmax.z-Parent->Box.AABBmin.z) / 2);
			}
			else
			{
				//bottom-left
				NewNode->Box.AABBmin.x = Parent->Box.AABBmin.x;
				NewNode->Box.AABBmin.y = -1000;
				NewNode->Box.AABBmin.z = Parent->Box.AABBmin.z + ((Parent->Box.AABBmax.z-Parent->Box.AABBmin.z) / 2);
				
				NewNode->Box.AABBmax.x = Parent->Box.AABBmax.x  - ((Parent->Box.AABBmax.x-Parent->Box.AABBmin.x) / 2);
				NewNode->Box.AABBmax.y = 1000;
				NewNode->Box.AABBmax.z = Parent->Box.AABBmax.z ;


			}

		}
		else // Covers top or bottom right
		{
		
			if(Top)
			{
				//Top-right
				NewNode->Box.AABBmin.x = Parent->Box.AABBmin.x + ((Parent->Box.AABBmax.x-Parent->Box.AABBmin.x) / 2);
				NewNode->Box.AABBmin.y = -1000;
				NewNode->Box.AABBmin.z = Parent->Box.AABBmin.z;
				
				NewNode->Box.AABBmax.x = Parent->Box.AABBmax.x ;
				NewNode->Box.AABBmax.y = 1000;
				NewNode->Box.AABBmax.z = Parent->Box.AABBmax.z - ((Parent->Box.AABBmax.z-Parent->Box.AABBmin.z) / 2);
			}
			else
			{
				//bottom-right
				NewNode->Box.AABBmin.x = Parent->Box.AABBmin.x + ((Parent->Box.AABBmax.x-Parent->Box.AABBmin.x) / 2);
				NewNode->Box.AABBmin.y = -1000;
				NewNode->Box.AABBmin.z = Parent->Box.AABBmin.z + ((Parent->Box.AABBmax.z-Parent->Box.AABBmin.z) / 2);
				
				NewNode->Box.AABBmax.x = Parent->Box.AABBmax.x ;
				NewNode->Box.AABBmax.y = 1000;
				NewNode->Box.AABBmax.z = Parent->Box.AABBmax.z ;


			}
		}
	}

	//no parent, must be the root node
	else
	{
		NewNode->Box.AABBmin.x = 0;
		NewNode->Box.AABBmin.y = -1000;
		NewNode->Box.AABBmin.z = 0;
		NewNode->Box.AABBmax.x = (Real)Width * Spacing ;
		NewNode->Box.AABBmax.y = 1000;
		NewNode->Box.AABBmax.z = (Real)Height * Spacing;
	}

	NewNode->ObjectBox = NewNode->Box;
	NewNode->ObjectBox.AABBmin.y = 10000;
	NewNode->ObjectBox.AABBmax.y =-10000;




	//If this is the maximum depth, add objects to it.
	if( CurrentDepth == MaxDepth )
	{

		//Create a vertex buffer for this particular spot
		CreateVertexbuffer( NewNode );
		//NewNode->GenerateTexture(*ScreenPtr, this, 0, 0, 0, 0);

		//Calculate the fluid matrix if the map is fluid-like
		if( FluidMap )
			NewNode->CalcFluidMatrix(this);

		//Insert a reference to this node in the reference
		//array.
		int ix = (int) ((Real)NewNode->Box.AABBmin.x / ((Real)(NodeWidth-1)*Spacing));
		int iy = (int) ((Real)NewNode->Box.AABBmin.z / ((Real)(NodeHeight-1)*Spacing));

		NodeReferenceList[ix+iy*nAcross].Node = NewNode;

		// For each object, test to see if
		// it's in the node, and shove it in
		for(int i=0; i< nObjects ; i++)
		{
			
			if( ObjectList[i] )
			{
				if(NewNode->Box.SphereContained( ObjectList[i]->Sphere )|| NewNode->Box.BoxContained( ObjectList[i]->Box )  )
				{
					NewNode->SceneList.push_back( i );
					
					

					//Adjust the bounding box of the node if an object stretches it
					if(ObjectList[i]->Location.y < NewNode->ObjectBox.AABBmin.y)
						NewNode->ObjectBox.AABBmin.y = ObjectList[i]->Location.y;

					if(ObjectList[i]->Location.y+ObjectList[i]->GetHeight() > NewNode->ObjectBox.AABBmax.y)
						NewNode->ObjectBox.AABBmax.y = ObjectList[i]->Location.y+ObjectList[i]->GetHeight();
					
					ObjectList[i]->InTree = true;
				}

				
			}
			else
			{
		        continue;
			}
		}
	}
	else
	{
	
		// For each object, test to see if
		// it's in the node, and shove it in
		for(int i=0; i< nObjects ; i++)
		{
			
			if( ObjectList[i] )
			{
				Real heightlevel = GetHeightValue( ObjectList[i]->Location );
				ObjectList[i]->Location.y = heightlevel;
				ObjectList[i]->AdjustBoundingBox();

				if(NewNode->Box.SphereContained( ObjectList[i]->Sphere )   )
				{
					//Adjust the bounding box of the node if an object stretches it
					if(ObjectList[i]->Location.y < NewNode->ObjectBox.AABBmin.y)
						NewNode->ObjectBox.AABBmin.y = ObjectList[i]->Location.y;

					if(ObjectList[i]->Location.y+ObjectList[i]->GetHeight() > NewNode->ObjectBox.AABBmax.y)
						NewNode->ObjectBox.AABBmax.y = ObjectList[i]->Location.y+ObjectList[i]->GetHeight();
				}

				
			}
		}
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

Real WorldManager::GetHeightValue( Position &ObjectLocation, Real* Slope, Vector3D* SlopeNormal, D3DXPLANE* TPlane, Vector3D* RoundSlopeNormal  )
{
		//declarations
	D3DXPLANE TrianglePlane;
	int Triangle=0; // 0 == bottom left triangle, 1 = top right triangle
	Real ReturnValue = ObjectLocation.y;

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
		List[0].y = Map[mx+my*Width];
		List[0].z = (Real) my*Spacing;

		List[1].x = (Real) mx*Spacing;
		List[1].y = Map[mx+((my+1)*Width)];
		List[1].z = (Real) (my+1)*Spacing;

		List[2].x = (Real) (mx+1)*Spacing;
		List[2].y = Map[(mx+1)+(my*Width)];
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

			SlopeNormal->Normalize();
		}

		//return the y value
		ReturnValue = Intersection.y;


	}

	//top right triangle
	else if(Triangle == 1)
	{
		

		List[0].x = (Real) mx*Spacing;
		List[0].y = Map[mx+((my+1)*Width)];
		List[0].z = (Real) (my+1)*Spacing;

		List[1].x = (Real) (mx+1)*Spacing;
		List[1].y = Map[(mx+1)+((my+1)*Width)];
		List[1].z = (Real) (my+1)*Spacing;

		List[2].x = (Real) (mx+1)*Spacing;
		List[2].y = Map[(mx+1)+my*Width];
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

			SlopeNormal->Normalize();
		}


		//return the y value
		ReturnValue = Intersection.y;
	}




	//Check to see if the "water" is solid, and if so, if we're below it.
	//If we are, move the y above it.
	Real WaterHeight;
	if(WaterWM)
	{
		if(WaterWM->SolidGround)
		{
			Real WaterSlope;
			Vector3D WaterSlopeNormal;
			D3DXPLANE WaterPlane;

			WaterHeight = WaterWM->GetHeightValue( ObjectLocation, &WaterSlope, &WaterSlopeNormal, &WaterPlane );

			if(WaterHeight > ReturnValue)
			{
				ReturnValue = WaterHeight;
				if(Slope)
					*Slope = WaterSlope;
				if(SlopeNormal)
					*SlopeNormal = WaterSlopeNormal;
				if(TPlane)
					*TPlane = WaterPlane;
			}
		}
	}
	


	return ReturnValue;
}



bool WorldManager::CheckForCollision( Vector3D& p1, Vector3D& p2,  CollisionInfo* Collision, bool CheckPlayer )
{


	//Find the node that the point is in, then check the node
	//for a collision.
	int quadx, quady;
    quadx = (int)((p1.x ) / ((Real)(NodeWidth-1)*Spacing));
	quady = (int)((p1.z ) / ((Real)(NodeWidth-1)*Spacing));

	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] != NULL)
		{
			if(DynamicObjectList[i]->Collision( p1, p2, Collision) )
				return true;
		}
	}

	if(CheckPlayer)
	{
		if(LocalPlayer.Collision( p1, p2, Collision) )
			return true;
	}


	return CheckNodeForCollision( p1, p2, Collision, NodeReferenceList[quadx+quady*nAcross].Node);


}


bool WorldManager::CheckForSphereCollision( Vector3D& p1, Real radius,  CollisionInfo* Collision, bool CheckPlayer )
{


	//Find the node that the point is in, then check the node
	//for a collision.
	int quadx, quady;
    quadx = (int)((p1.x ) / ((Real)(NodeWidth-1)*Spacing));
	quady = (int)((p1.z ) / ((Real)(NodeWidth-1)*Spacing));

	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] != NULL)
		{
			if(DynamicObjectList[i]->SphereCollision( p1, radius, Collision) )
				return true;
		}
	}

	if(CheckPlayer)
	{
		if(LocalPlayer.SphereCollision( p1, radius, Collision) )
			return true;
	}


	return CheckNodeForSphereCollision( p1,radius , Collision, NodeReferenceList[quadx+quady*nAcross].Node);


}

bool WorldManager::CheckNodeForCollision(  Vector3D& p1, Vector3D& p2,  CollisionInfo* Collision, QuadNode* Node )
{
	//Check to see if the point in question is in the box
	/*bool InBox = Node->Box.PointContained( p1 );
	bool RVal=false;


	//First, check to see if the object is even in this node.
	//If it's not just take an early out.
	if(!InBox)
	{
		return false;
	}*/


	//It's in the node, so proceed onto other tests
	//else
	{
		//test all the objects to see if the line hits them.
		//all the objects have early out box/sphere checks, so
		//we don't need to do that here.
	
		for(int i=0; i< Node->SceneList.size(); i++)
		{	
			SceneObject* object = ObjectList[Node->SceneList[i]];

			if(object->Collision(p1, p2,  Collision))
				return true;
		}
	}
    	
	return false;

}



bool WorldManager::CheckNodeForSphereCollision(  Vector3D& p1, Real radius,  CollisionInfo* Collision, QuadNode* Node )
{
	//Check to see if the point in question is in the box
	bool InBox = Node->Box.PointContained( p1 );
	bool RVal=false;


	//First, check to see if the object is even in this node.
	//If it's not just take an early out.
	if(!InBox)
	{
		return false;
	}


	//It's in the node, so proceed onto other tests
	else
	{
		//test all the objects to see if the line hits them.
		//all the objects have early out box/sphere checks, so
		//we don't need to do that here.
	
		for(int i=0; i< Node->SceneList.size(); i++)
		{	
			SceneObject* object = ObjectList[Node->SceneList[i]];

			if(object->SphereCollision(p1, radius,  Collision))
				return true;
		}
	}
    	
	return false;

}


void _fastcall WorldManager::DrawTree ( QuadNode* Node, bool ParentFullyVisible )
{
	int result=0;
	
	if(!ParentFullyVisible)
		result = ScreenPtr->CheckAABB( Node->Box );
	else
		result = 2;

	RecursionCount++;

	CurrentDepth ++;

	//box isn't even visible, cancel drawing
	if(result == 0)
	{
		result = ScreenPtr->CheckAABB( Node->ObjectBox );

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

			
			if(result != 3 && !Node->Invisible )
			{

				//Draw the terrain
				Matrix M;
				ScreenPtr->SetTransform(M);
				ScreenPtr->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(10, 10, 10, 255));

				
				ScreenPtr->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				ScreenPtr->SetRenderState(D3DRS_LIGHTING, TRUE);
								
		
				//ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				ScreenPtr->SetRenderState( D3DRS_SRCBLEND, SrcAlpha );
				ScreenPtr->SetRenderState( D3DRS_DESTBLEND, DestAlpha );
			

				if(NoCulling || !SolidGround)
					ScreenPtr->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				

				ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
				ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
				ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			
				
				ScreenPtr->SetTexture( DetailMapSigned, 1);
				ScreenPtr->SetTexture( hGrassTexture, 0);			
				ScreenPtr->DrawPrimitiveVB( Node->MapVB, &Node->GrassData.IB );
				ScreenPtr->SetTexture( hCliffTexture, 0);
				ScreenPtr->DrawPrimitiveVB( Node->MapVB, &Node->CliffData.IB );

				ScreenPtr->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				ScreenPtr->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );

			}

			ScreenPtr->SetTexture( -1, 1 );
			ScreenPtr->SetTextureMode( TMNormal );
			ScreenPtr->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA((int)WorldWeather.IntermediateState.AR, (int)WorldWeather.IntermediateState.AG, (int)WorldWeather.IntermediateState.AB, 255));
			ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );



			ScreenPtr->SetTextureMode( TMDarkMap );
			ScreenPtr->SetTexture( ObjectDetailMap, 1 );
			//Draw all visible objects in this node
			for(int i=0; i< Node->SceneList.size(); i++)
			{	
				SceneObject* object = ObjectList[Node->SceneList[i]];

				if( object->CheckVisibility( *ScreenPtr ) && object->Parent == NULL)
				{
					object->Draw( *ScreenPtr, *MMPtr, Viewer, *GTPtr );
					ObjectsDrawn++;
				}

				ObjectsTested ++;
			}




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
	

void WorldManager::DrawWater(Screen3D& Screen )
{		

	return;
	CustomVertex Water[4];

	Water[0].position.x=-(Real)Width*Spacing+800;
	Water[0].position.y=120;
	Water[0].position.z=-(Real)Height*Spacing-800;
	Water[0].normal.x = 0;
	Water[0].normal.y = 1;
	Water[0].normal.z = 0;
	Water[0].tu       = 0;
	Water[0].tv       = 0;
	Water[0].tu2      = 0;
	Water[0].tv2      = 0;
	

	Water[1].position.x=(Real) Width*Spacing+800;
	Water[1].position.y=120;
	Water[1].position.z=-(Real)Height*Spacing-800;
	Water[1].normal.x = 0;
	Water[1].normal.y = 1;
	Water[1].normal.z = 0;
	Water[1].tu       = (Real)Width;
	Water[1].tv       = 0;
	Water[1].tu2      = 4;
	Water[1].tv2      = 0;
	
	Water[2].position.x=-(Real)Width*Spacing-800;
	Water[2].position.y=120;
	Water[2].position.z=(Real)Height*Spacing+800;
	Water[2].normal.x = 0;
	Water[2].normal.y = 1;
	Water[2].normal.z = 0;
	Water[2].tu       = 0;
	Water[2].tv       = (Real)Height;
	Water[2].tu2      = 0;
	Water[2].tv2      = 4;
	
	Water[3].position.x=(Real) Width*Spacing+800;
	Water[3].position.y=120;
	Water[3].position.z=(Real) Height*Spacing+800;
	Water[3].normal.x = 0;
	Water[3].normal.y = 1;
	Water[3].normal.z = 0;
	Water[3].tu       = (Real)Width;
	Water[3].tv       = (Real)Height;
	Water[3].tu2      = 4;
	Water[3].tv2      = 4;
	


	// draw the water

	//Screen.SetTextureMode( TMBlendMap );

	Screen.TM.SetTextureMode(TMNormal, 0, true);

	



	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_ONE);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
	Screen.D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	Screen.SetRenderState(D3DRS_FOGENABLE, FALSE);
	Screen.D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Screen.D3DDevice->SetVertexShader( D3DFVF_LANDSCAPEVERTEX );
	Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Water, sizeof(LandscapeVertex));
	Screen.SetRenderState(D3DRS_FOGENABLE, TRUE);
	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);

	Screen.TM.SetTextureMode(TMNormal, 0, true);

	Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
	
	Screen.D3DDevice->SetTexture(1, NULL);
}



SceneHandle WorldManager::AddDynamic( SceneObject* object )
{
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] == NULL)
		{
			DynamicObjectList[i] = object;
			return i;
		}
	}

	return -1;
}

void WorldManager::KillDynamic( SceneHandle Who )
{
	if(DynamicObjectList[Who])
	{
		delete DynamicObjectList[Who];
		DynamicObjectList[Who] = NULL;
	}
}

SceneObject* WorldManager::FindNearestObject( Position& Location, Real MaxDistance )
{
	Vector3D vLocation = Location;
	Vector3D vObjectVector, vTestVector;
	Real Distance;
	Real ShortestDistance = 1000000;
	SceneObject* ReturnObject = NULL;
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] != NULL)
		{
			if( DynamicObjectList[i]->Parent != NULL)
				continue;
			vObjectVector = DynamicObjectList[i]->Location;
			vTestVector = vObjectVector - vLocation;
			Distance = vTestVector.Mag();
			if( Distance <= MaxDistance && Distance <= ShortestDistance )
			{	
				ShortestDistance = Distance;
				ReturnObject = DynamicObjectList[i];
			}
		}
	}

	return ReturnObject;

}

void WorldManager::DrawDynamicObjects( Screen3D& Screen )
{
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] != NULL)
		{
			if(DynamicObjectList[i]->CheckVisibility( Screen ) && DynamicObjectList[i]->Parent == NULL )
			{
				DynamicObjectList[i]->Draw(Screen, *MMPtr, Viewer, *GTPtr );
				DynamicObjectList[i]->DrawInfo( Screen );
			}
		}
	}
}


SceneHandle WorldManager::AddNetEnt( M3DAddEntPacket* NewEnt )
{
	//Construct a generic filename from the entity's type
	char Filename[128];


	//Check for specific type and create a class of that type
    if(strcmpi(NewEnt->ClassName, "avatar")==0)
	{	
		strcpy(Filename, "..\\art\\characters\\");
		strcat(Filename, NewEnt->ClassName);
		strcat(Filename, ".ini");

		Avatar* Entity = new Avatar;
		Entity->Init( *ScreenPtr, this, *MMPtr, *AMPtr, Filename);
		Entity->ID = NewEnt->ID;

		NetworkObjectList.push_back( Entity );

		return Entity->ID;
	}
		//Check for specific type and create a class of that type
    if(strcmpi(NewEnt->ClassName, "NPC")==0)
	{	
		strcpy(Filename, "..\\art\\characters\\");
		strcat(Filename, NewEnt->SubType);
		strcat(Filename, ".ini");

		NPC* Entity = new NPC;
		Entity->Init( *ScreenPtr, this, *MMPtr, *AMPtr, Filename);
		Entity->ID = NewEnt->ID;

		NetworkObjectList.push_back( Entity );

		return Entity->ID;
	}
	else
	{
		strcpy(Filename, "..\\art\\objects\\decoration\\");
		strcat(Filename, NewEnt->ClassName);
		strcat(Filename, ".ini");

		SceneObject* Entity = new SceneObject;
		Entity->Init( *ScreenPtr, this, *MMPtr, *AMPtr, Filename);
		Entity->ID = NewEnt->ID;
	
		NetworkObjectList.push_back( Entity );

		return Entity->ID;
	}

	return -1;
}

void  WorldManager::KillNetEnt( SceneHandle Who )
{
	for(int i=0; i<NetworkObjectList.size(); i++)
	{
		if(NetworkObjectList[i]->ID == Who)
		{
			delete NetworkObjectList[i];
			NetworkObjectList.erase( NetworkObjectList.begin() + i );
		}
	}

}

SceneObject*  WorldManager::GetNetEnt( SceneHandle h )
{
	for(int i=0; i<NetworkObjectList.size(); i++)
	{
		if(NetworkObjectList[i]->ID == h)
		{
			return NetworkObjectList[i];
		}
	}


    return NULL;
}

void  WorldManager::DrawNetworkObjects( Screen3D& Screen )
{
	for(int i=0; i<NetworkObjectList.size(); i++)
	{
		if(NetworkObjectList[i]->CheckVisibility( Screen ) )
			NetworkObjectList[i]->Draw(Screen, *MMPtr, Viewer, *GTPtr );
	}

}

void WorldManager::DrawFinal( Screen3D& Screen )
{

	//Draw info O_O!
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if(DynamicObjectList[i] != NULL)
		{
			if(DynamicObjectList[i]->CheckVisibility( Screen, false ) )
			{
				DynamicObjectList[i]->DrawInfo( Screen );
			}
		}
	}

	//Draw movie dialog something or other
	if(InMovie)
	{
		RECT TopRect, BotRect;
		TopRect.left   = 0;
		TopRect.top    = 0;
		TopRect.right  = Screen.GetWidth();
		TopRect.bottom = 80;

		BotRect.left   = 0;
		BotRect.top    = Screen.GetHeight()-80;
		BotRect.right  = Screen.GetWidth();
		BotRect.bottom = Screen.GetHeight();
		
		TextureHandle BlackBox = Screen.GetTextureHandle("..\\art\\ui\\black.bmp");

		Screen.DrawSprite( TopRect, BlackBox, true);
		Screen.DrawSprite( BotRect, BlackBox, true);

		FontHandle f = Screen.FM.GetFont( "verdana", 8);
		Font2D* DrawFont  = Screen.FM.GetFontPointer( f );

		DrawFont->Draw( Screen, 10, 0, false, D3DCOLOR_XRGB(255, 255, 255), (char*)TopMovieText.c_str() );
		DrawFont->Draw( Screen, 10, Screen.GetHeight()-60+2, false, D3DCOLOR_XRGB(255, 255, 255), (char*)BottomMovieText.c_str() );
	}


}
