#ifndef _3DCARD_H_
#define _3DCARD_H_

// I N C L U D E S ///////////////////////////////////////////////////////
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "errors.h" //error list
#include "texturemanager.h"
#include "types.h"

//forward declarations
class Screen3D;


// D E F I N E S ////////////////////////////////////////////////////////


// C L A S S E S ////////////////////////////////////////////////////////


struct ScreenPoint3
{
	float x, y, z;
	ScreenPoint3 () { x=0; y=0; z=0; }
};

//////// Texture //////////////////////////////
//
// Description:
//
// Represents a OpenGL texture.
//
// Inherits From:
//
// - Nothing
//
// TODO:
//
// - Make bitmap loading code cleaner, more reliable.
//
// Known Bugs:
//
// - 
//
///////////////////////////////////////////////


class Texture
{
protected:

	//Texture handle 
	GLuint TextureSurface;
	bool Initialized;


	//Filename
	char TextureFile[64];

	TextureType Type;
	int IntendedStage;

	//classes whcih need to have full access to this.
	friend class Screen3D;

	
public:

	//constructor/destructor
	Texture() { TextureSurface = NULL; IntendedStage = 0; Type = TNormal; Initialized = false; }
	~Texture() { Destroy(); }


	//Function to load Textures
	S3DLIBRESULT LoadTexture(Screen3D& Screen, char *filename, char* alphamask=NULL);
	void Destroy();

	//Accessor functions	
	char* GetFilename() { return TextureFile; }

};



//////// Screen3D /////////////////////////////
//
// Description:
//
// Represents a opengl device. Use this for drawing
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

	HDC   hDC; 
	HGLRC hRC;

	HWND hWnd; //window to be drawn in
	
	TextureManager TM;

	//viewport variables
	int ViewX, ViewY, ViewWidth, ViewHeight;

	float frustum[6][4];


public:
	Screen3D() { ViewX = ViewY = ViewWidth = ViewHeight = 0; }
	~Screen3D() { Destroy(); }

	//init
	S3DLIBRESULT Create(HWND hwnd, float viewdistance);
	void Destroy();

	void ExtractFrustum();
	void SetViewport(int x=-1, int y=-1, int Width=-1, int Height=-1);

	bool PointInFrustum( float x, float y, float z );
	int  SphereInFrustum( float x, float y, float z, float radius );
	int  CubeInFrustum( float x, float y, float z, float size );
	bool PolygonInFrustum( int numpoints, ScreenPoint3* pointlist );
	int  CheckAABB(AABB& AABBox);
	
	//drawing
	void Clear() ;
	void UpdateScreen();


	
	//material and texture stuff
	TextureHandle GetTextureHandle(char* filename, char *alphamask=NULL);
	S3DLIBRESULT  SetTexture(Texture* texture, int stage) ;
	S3DLIBRESULT  SetTexture(TextureHandle h, int Stage);
	//void          SetMaterial ( MeshMaterial* Mat ) ;
	
};




#endif
