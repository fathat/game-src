#ifndef _3DCARD_H_
#define _3DCARD_H_

// I N C L U D E S ///////////////////////////////////////////////////////
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "errors.h" //error list
#include "texturemanager.h"
#include "types.h"
#include "font.h"
#include "VertexTypes.h"
#include "Texture.h"
#include "Overlay.h"

//forward declarations
class Screen3D;
class VertexBuffer;
class IndexBuffer;
struct MeshMaterial;
class Texture;
class VertexShader;

// D E F I N E S ////////////////////////////////////////////////////////

#define Real float

//return values
/*enum S3DLIBRESULT { S3DLIB_OK,  //Function successful
				    S3DLIB_ERR, //Generic error
					S3DLIB_INVALIDFLAGS, //Conflicting or unsupported flags
					S3DLIB_BADPARAMETER};*/


// capability list

enum DeviceCaps { S3DDev_Hardware,    //device can draw triangles in hardware
				  S3DDev_PureDevice,  //device supports t&l and shaders
				  S3DDev_MaxLights,   //number of simultaneous lights
				  S3DDev_MaxTextureWidth,
				  S3DDev_MaxTextureHeight,
				  S3DDev_MaxTextureStages,
				  S3DDev_MaxPointSize
};

enum S3DDeviceState { S3D_OK, S3D_LOST, S3D_RESETABLE };



enum S3DHardwareTLSupport { S3D_NONE, S3D_STANDARD, S3D_VS_1_0,  S3D_VS_1_1 };


// C L A S S E S ////////////////////////////////////////////////////////



//Stores info about a certain display mode.
struct DisplayModeInfo
{
	int Width, Height, BPP;
	int RefreshRate;
	bool Fullscreen;
	bool UseHardwareTL;
	D3DFORMAT BackBufferFormat;

	//generate human readable string for
	//device mode
	DisplayModeInfo() { UseHardwareTL = false; }
	void PrintInfo( char* outstring );
};





struct RenderBlock
{
	//matrix for where the object is
	D3DXMATRIX m;

	VertexBuffer* VB;
	IndexBuffer*  IB;

	//Used to roughly figure out draw order. Any distance below zero
	//will be drawn sequencially after everything above zero is rendered.
	Real DistFromCamera;
};




//////// Screen3D /////////////////////////////
//
// Description:
//
// Represents a D3D8 device. Use this for drawing
// polygons and anything other 3D related.
//
// Known Problems:
//
// - None
//
// TODO:
//
//
///////////////////////////////////////////////

class Screen3D
{
public: //stuff which is needs to be public for now

	LPDIRECT3D9             D3D; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       D3DDevice; // Our rendering device

	HWND hWnd; //window to be drawn in

	DisplayModeInfo DisplayMode;
	D3DPRESENT_PARAMETERS d3dpp;

	
	//Describes what the card can do for hardware T&L and such
	S3DHardwareTLSupport TLSupport;

	TextureManager TM;
	FontManager FM;

	//FOV planes
	Plane3D FrustumPlanes[6];//frustum planes extracted from 
					//view matrix X projection matrix
	
	int FOV;
	int ScreenViewDepth;

	int InitialAvailableMem;

	int R,G,B,A;
	int PolygonsRendered;
	int DrawCalls;

	bool AlphaRendering;

	std::list<RenderBlock> DrawList;

	S3DDeviceState DeviceState;

	bool m_wireframe;


protected:
	CRITICAL_SECTION screenCriticalSection;

	//Overlays to be drawn
	std::list< Overlay > OverlayQueue;

	void FlushOverlays();

	Matrix m_projectionMatrix;
	Matrix m_viewMatrix;
	Matrix m_worldMatrix;

public:
	Screen3D();
	~Screen3D() { Destroy(); DeleteCriticalSection ( &screenCriticalSection ) ;}

	/*int GetModeListSize(); //returns the number of display modes
	DisplayModeInfo GetModeListIndex( int index); //returns a specific display mode
	int FindClosestMode( DisplayModeInfo &DM );
	bool ModeExists( DisplayModeInfo &DM );*/

	//init
	bool Create(HWND hwnd, DisplayModeInfo& m, float viewdistance);
	void Destroy();

	void SetViewport(int x, int y, int width, int height);
	void SetClearColor( int r, int g, int b, int a ) { R=r;G=g;B=b; A=a; }
	void TakeScreenshot(char* basefilename);

	
	void LockScreenAccess()
	{
		EnterCriticalSection( &screenCriticalSection );
	}
	void UnlockScreenAccess()
	{
		LeaveCriticalSection( &screenCriticalSection );
	}

	//visibility tests 
	void CalculateFOV ();//extract the frustum planes
	bool CheckPointVisibility(Point3D& Point);//checks a single points visibility
	
	
	//Screen space things
	void GetMouseRay(int x, int y, Vector3D* OutRayOrigin, Vector3D* OutRayDir ); // generates a ray from where the mouse is
	void GetScreenCoordinates( Vector3D* Out, Vector3D& Input );

	
	//information
	int GetDeviceCapability ( DeviceCaps CapToCheck );

	//different frustum tests
	int BoxInFrustum(BoundingBox& BBox);
	int SphereInFrustum(BoundingSphere& BSphere);
	int CubeInFrustum(BoundingCube& BCube);
	int CheckAABB(AABB& AABBox);//axis aligned bounding box
	
	//How far you can see
	bool ChangeViewDepth(Real ViewDepth);
	int  ViewDistance( ) { return ScreenViewDepth; }
	
	//player's field of view, in degrees
	void SetFieldOfView( int newFOV );

	//drawing
	void Clear(bool ZOnly=false) ;
	void UpdateScreen(bool StretchToFit=true);


	void SetFVF( DWORD FVF ) { D3DDevice->SetVertexShader( NULL ); D3DDevice->SetFVF( FVF ); }


	void Wireframe(bool on_or_off);

	//texture filtering
	void SetPointTextureFiltering(int stage=0, int endstage=-1);
	void SetLinearTextureFiltering(int stage=0, int endstage=-1);
	void SetAnisotropicTextureFiltering( int stage=0, int endstage=-1);
	void SetTextureFiltering( DWORD filter, int stage=0, int endstage=-1);

	void SetStreamSource( VertexBuffer* vb, int strm=0) ;

	//vertex shader
	void SetVertexShader( IDirect3DVertexShader9* vs ) ;

	void SetVertexShaderConstant( int reg, const D3DXVECTOR4* constant, int count);
	void SetVertexShaderConstant( int reg, const D3DXMATRIX* constant) ;

	//3d drawing calls
	void Start3D();
	void End3D();

	int GetWidth() { return DisplayMode.Width;}
	int GetHeight() { return DisplayMode.Height;}

	int GetUsedMemory() { return InitialAvailableMem -  D3DDevice->GetAvailableTextureMem(); }
	
	//draws text
	void ShowText(int x, int y, char string[512]);

	//material and texture stuff
	TextureHandle GetBlankTexture( const char* name, const char* alphamask,  int width=256, int height=256 );
	TextureHandle GetTextureHandle(const char* filename, const char *alphamask=NULL);
	void		  SetTexture(Texture* texture, int stage) ;
	void		  SetTexture(TextureHandle h, int Stage);
	void          SetMaterial ( D3DMATERIAL9* Mat ) ;
	void		  DefaultMaterial();
	void		  SetTextureMode( TextureMode tm, int BaseStage=0 ) {
		TM.SetTextureMode(tm, BaseStage);
	}
	void		  SetRenderState( D3DRENDERSTATETYPE State, DWORD Value ) { D3DDevice->SetRenderState( State, Value) ; }

	//matrix transformation calls (!)
	void SetTransform ( Matrix& M );
	void SetTransform ( D3DXMATRIX& m );
	void MultTransform ( Matrix& M );
	Matrix& WorldMatrix() { return m_worldMatrix; } 
	
	void SetViewTransform ( Matrix& M, bool CalcFOV=true );
	Matrix& ViewMatrix() { return m_viewMatrix; }
	
	void   SetProjectionMatrix( Matrix& M );
	Matrix& ProjectionMatrix() { return m_projectionMatrix; }

	void EnableWireframeDrawing();	
	void EnableSolidDrawing();
	void ResetWireframeState();

	void BlankArea( RECT& Dimensions);
	void DrawSpriteRect( RECT& Dimensions, TextureHandle Image, bool StretchTexture, D3DCOLOR color=0xFFFFFFFF);
	void DrawSpriteNoHandle( RECT& Dimensions, Texture* Image, bool StretchTexture, D3DCOLOR color, bool SetAlphaStates=true);
	void DrawSpritePoints( Point3D* pointlist, TextureHandle Image );
	void DrawSprite( int x, int y, Texture* Image, int alpha=255);
	void DrawSprite( int x, int y, TextureHandle Image, int alpha=255);

	void DrawRectangle( int x, int y, int w, int h, D3DCOLOR color );

	void PostOverlay( int x, int y, TextureHandle image, float alpha = 1.0f );
	
	//Draws a Vertex Buffer set with SetVertexSource. Fails if the vertex
	//source is null. You can optionally also provide an IndexBuffer
	void DrawPrimitiveVB ( VertexBuffer* VB, IndexBuffer* IB = NULL, _D3DPRIMITIVETYPE PrimType=D3DPT_TRIANGLELIST,  DWORD VertexShader=D3DFVF_CUSTOMVERTEX, int Offset=0, int Count=0 );
	
	void DrawAllAlpha();
	void DrawAlphaPrimitive( VertexBuffer* VB, Position& camera, Position& CentralLocation, IndexBuffer* IB = NULL  );
	
};

#endif
