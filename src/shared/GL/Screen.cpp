#include "screen.h"
#include <math.h>

//GLOBALS//////////////////////////////////////////////////////////////
extern int redbits, greenbits, bluebits, rgbbits; //Work well as globals

S3DLIBRESULT Screen3D::Create(HWND hWnd, float viewdistance)
{
	// Store the device context
	hDC = GetDC(hWnd);		

	// Select the pixel format
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		16,								// Want 16bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		32,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);

	// Create the rendering context and make it current
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);


	//init the texture manager
	TM.Init(this);

	//return OK
	return S3DLIB_OK;

}

void Screen3D::Destroy()
{
	

}


void Screen3D::ExtractFrustum()
{
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;

   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;

   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;

   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}


bool Screen3D::PointInFrustum( float x, float y, float z )
{
   int p;

   for( p = 0; p < 6; p++ )
      if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 )
         return false;
   return true;
}

int Screen3D::SphereInFrustum( float x, float y, float z, float radius )
{
   int p;
   int c = 0;
   float d;

   for( p = 0; p < 6; p++ )
   {
      d = frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3];
      if( d <= -radius )
         return 0;
      if( d > radius )
         c++;
   }
   return (c == 6) ? 2 : 1;
}

bool Screen3D::PolygonInFrustum( int numpoints, ScreenPoint3* pointlist )
{
   int f, p;

   for( f = 0; f < 6; f++ )
   {
      for( p = 0; p < numpoints; p++ )
      {
         if( frustum[f][0] * pointlist[p].x + frustum[f][1] * pointlist[p].y + frustum[f][2] * pointlist[p].z + frustum[f][3] > 0 )
            break;
      }
      if( p == numpoints )
         return false;
   }
   return true;
}


int Screen3D::CubeInFrustum( float x, float y, float z, float size )
{
   int p;
   int c;
   int c2 = 0;

   for( p = 0; p < 6; p++ )
   {
      c = 0;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         c++;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         c++;
      if( c == 0 )
         return 0;
      if( c == 8 )
         c2++;
   }
   return (c2 == 6) ? 2 : 1;
}



int Screen3D::CheckAABB(AABB& AABBox)
{
	
AABB MinMaxExtremes;
bool intersect = false;
for(int i=0;i<6;i++)
	{
	for(int j=0;j<3;j++)
		{
		if (frustum[i][j] < 0.0f)
			{
			MinMaxExtremes.AABBmin[j] = AABBox.AABBmin[j];
			MinMaxExtremes.AABBmax[j] = AABBox.AABBmax[j];
			}
		else{
			MinMaxExtremes.AABBmin[j] = AABBox.AABBmax[j];
			MinMaxExtremes.AABBmax[j] = AABBox.AABBmin[j];
			}
		}
		;
	if(((MinMaxExtremes.AABBmin.x*frustum[i][0])+(MinMaxExtremes.AABBmin.y*frustum[i][1])+(MinMaxExtremes.AABBmin.z*frustum[i][2])+frustum[i][3]) < 0.0f) return 0;
	if(((MinMaxExtremes.AABBmax.x*frustum[i][0])+(MinMaxExtremes.AABBmax.y*frustum[i][1])+(MinMaxExtremes.AABBmax.z*frustum[i][2])+frustum[i][3])<=0.0f)intersect = true;
	}
if(intersect)return 1;
return 2;
}



void Screen3D::SetViewport(int x, int y, int width, int height)
{
	//set the window width
	if(x != -1)
		ViewX = x;

	if(y != -1)
		ViewY = y; 

	if(width != -1)
		ViewWidth  = width;
	if(height != -1)
		ViewHeight = height;

	// Set the viewport to be the entire window
    glViewport(x, y, ViewWidth, ViewHeight);

	glMatrixMode(GL_PROJECTION);

	// Reset the coordinate system before modifying
    glLoadIdentity();

	//set projection
	gluPerspective(90, ViewWidth/ViewHeight, 1, 1000);

	//set initial render states
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	//use a left handed coordinate system instead of a right handed one
	glScalef(1, 1, -1);


}




void Screen3D::Clear()
{
	glClearColor(0, .5, 1, 1);
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}

void Screen3D::UpdateScreen()
{
	SwapBuffers(hDC);
}


TextureHandle Screen3D::GetTextureHandle(char* filename, char* alphamask)
{
	TextureHandle i = TM.TextureExists(filename);

	if(i == -1)
	{
		return TM.AddTexture(filename, alphamask);
	}

	return i;

}


S3DLIBRESULT Screen3D::SetTexture(Texture* texture, int stage) 
{
	if(texture == NULL)
	{
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->TextureSurface);		
	}

	return S3DLIB_OK;
}


S3DLIBRESULT Screen3D::SetTexture(TextureHandle h, int Stage)
{
	return SetTexture(TM.GetTexture(h), Stage);
}



/*void Screen3D::SetMaterial(MeshMaterial* Mat)
{
	if(Mat == NULL)
	{
		//using this temp variable to avoid an ambiguous call.
		Texture* T=NULL;
		SetTexture(T, 0);
		return;
	}

	SetTexture(Mat->Image, 0);
}*/


