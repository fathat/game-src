#include "sky.h"
#include "..\defs\defs.h"

bool Sky::Init( Screen3D& Screen, char* WeatherFilename  )
{

	// load the textures.
	char TempBuffer[255], TempBuffer2[255];
	GetIniValue(WeatherFilename, "graphics", "top_sky", TempBuffer);
	GetIniValue(WeatherFilename, "graphics", "top_sky_alpha", TempBuffer2);
	TopTexture = Screen.GetTextureHandle(TempBuffer, TempBuffer2 );

	GetIniValue(WeatherFilename, "graphics", "bot_sky", TempBuffer);
	BottomTexture = Screen.GetTextureHandle(TempBuffer );

	GetIniValue(WeatherFilename, "graphics", "sky_north", TempBuffer);
	North = Screen.GetTextureHandle(TempBuffer);
	GetIniValue(WeatherFilename, "graphics", "sky_south", TempBuffer);
	South = Screen.GetTextureHandle(TempBuffer);
	GetIniValue(WeatherFilename, "graphics", "sky_east", TempBuffer);
	East = Screen.GetTextureHandle(TempBuffer);
	GetIniValue(WeatherFilename, "graphics", "sky_west", TempBuffer);
	West = Screen.GetTextureHandle(TempBuffer);

	GetIniValue(WeatherFilename, "graphics", "blend_texture", TempBuffer);
	BlendTexture = Screen.GetTextureHandle(TempBuffer, TempBuffer);

	

	

	return true;
}

bool Sky::Init( Screen3D* screen, TiXmlElement* skynode )
{

	North		  = screen->GetTextureHandle( xmlreads( skynode, "North" ) );
	South		  = screen->GetTextureHandle( xmlreads( skynode, "South" ) );
	East		  = screen->GetTextureHandle( xmlreads( skynode, "East" ) );
	West		  = screen->GetTextureHandle( xmlreads( skynode, "West" ) );
	TopTexture    = screen->GetTextureHandle( xmlreads( skynode, "Up" ) );
	BottomTexture = screen->GetTextureHandle( xmlreads( skynode, "Down" ) );

	return true;
}

void Sky::Draw( Screen3D& Screen, Camera& Viewer, Real r, Real g, Real b )
{
	

	Screen.SetRenderState(D3DRS_ZENABLE, FALSE);
	Screen.SetRenderState(D3DRS_LIGHTING, TRUE);
	Screen.SetRenderState(D3DRS_FOGENABLE, FALSE);
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Screen.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	Screen.SetFVF(D3DFVF_CUSTOMVERTEX);

	Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );
	Screen.SetMaterial( NULL);
	Screen.SetViewTransform( Viewer.GetRotationMatrix() );
	Matrix Identity;
	Screen.SetTransform(Identity);

	CustomVertex p[4];

	//top
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 1;
	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 0;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 1;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 0;
	
	if(TopTexture > -1 )
	{
		Screen.SetTexture( TopTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));

	}

	

	//first face

	Screen.SetTexture( North, 0 );
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = -SKY_EXTENTS;
	p[1].position.z   = SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = -SKY_EXTENTS;
	p[3].position.z   = SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;

	if(North > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/

	}

	//second face
	Screen.SetTexture( East, 0 );
	p[0].position.x   = SKY_EXTENTS;
	p[0].position.y   = SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = SKY_EXTENTS;
	p[1].position.y   = -SKY_EXTENTS;
	p[1].position.z   = SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = SKY_EXTENTS;
	p[2].position.z   = -SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = -SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
		
	if(East > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		
		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}

	//third face
	Screen.SetTexture( South, 0 );

	p[0].position.x   = SKY_EXTENTS;
	p[0].position.y   = SKY_EXTENTS;
	p[0].position.z   = -SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = SKY_EXTENTS;
	p[1].position.y   = -SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = -SKY_EXTENTS;
	p[2].position.y   = SKY_EXTENTS;
	p[2].position.z   = -SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = -SKY_EXTENTS;
	p[3].position.y   = -SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
	
	if(South > -1 )
	{

		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}
	
	//fourth face

	Screen.SetTexture( West,0 );
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = SKY_EXTENTS;
	p[0].position.z   = -SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	

	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = -SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	
	

	p[2].position.x   = -SKY_EXTENTS;
	p[2].position.y   = SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
		

	p[3].position.x   = -SKY_EXTENTS;
	p[3].position.y   = -SKY_EXTENTS;
	p[3].position.z   = SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
	
	if(West > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));		

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}

	

	
	Screen.SetRenderState(D3DRS_LIGHTING, TRUE);
	Screen.SetRenderState(D3DRS_ZENABLE, TRUE);
	Screen.SetRenderState(D3DRS_FOGENABLE, TRUE);
}

void Sky::DrawReflected( Screen3D& Screen, Camera& Viewer, Real r, Real g, Real b )
{
	
	Screen.SetRenderState(D3DRS_ZENABLE, FALSE);
	Screen.SetRenderState(D3DRS_LIGHTING, FALSE);
	Screen.SetRenderState(D3DRS_FOGENABLE, FALSE);
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Screen.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_COLOROP, D3DTOP_MODULATE);
	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	//Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_COLOROP, D3DTOP_DISABLE);
	//Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_COLORARG1, D3DTA_TEXTURE);
	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Screen.D3DDevice->SetTextureStageState(0,  D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	Screen.SetFVF(D3DFVF_PARTICLEVERTEX);
	Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );
	Screen.SetMaterial( NULL);
	Screen.SetViewTransform( Viewer.GetRotationMatrix() );
	Matrix Identity;
	Screen.SetTransform(Identity);

	ParticleVertex p[4];

	//top
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = -SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 1;
	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = -SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 0;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = -SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 1;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = -SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 0;

	p[0].color = D3DCOLOR_ARGB( 255, 0, 0, 100 );
	p[1].color = D3DCOLOR_ARGB( 255, 0, 0, 100 );
	p[2].color = D3DCOLOR_ARGB( 255, 0, 0, 100 );
	p[3].color = D3DCOLOR_ARGB( 255, 0, 0, 100 );
	
	if(TopTexture > -1 )
	{
		Screen.SetTexture( TopTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(ParticleVertex));

	}

	

	//first face

	Screen.SetTexture( North, 0 );
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = -SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = SKY_EXTENTS;
	p[1].position.z   = SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = -SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = SKY_EXTENTS;
	p[3].position.z   = SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;

	if(North > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(ParticleVertex));

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/

	}

	//second face
	Screen.SetTexture( East, 0 );
	p[0].position.x   = SKY_EXTENTS;
	p[0].position.y   = -SKY_EXTENTS;
	p[0].position.z   = SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = SKY_EXTENTS;
	p[1].position.y   = SKY_EXTENTS;
	p[1].position.z   = SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = SKY_EXTENTS;
	p[2].position.y   = -SKY_EXTENTS;
	p[2].position.z   = -SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = SKY_EXTENTS;
	p[3].position.y   = SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
		
	if(East > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(ParticleVertex));
		
		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}

	//third face
	Screen.SetTexture( South, 0 );

	p[0].position.x   = SKY_EXTENTS;
	p[0].position.y   = -SKY_EXTENTS;
	p[0].position.z   = -SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	p[1].position.x   = SKY_EXTENTS;
	p[1].position.y   = SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	p[2].position.x   = -SKY_EXTENTS;
	p[2].position.y   = -SKY_EXTENTS;
	p[2].position.z   = -SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
	p[3].position.x   = -SKY_EXTENTS;
	p[3].position.y   = SKY_EXTENTS;
	p[3].position.z   = -SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
	
	if(South > -1 )
	{

		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(ParticleVertex));

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}
	
	//fourth face

	Screen.SetTexture( West,0 );
	p[0].position.x   = -SKY_EXTENTS;
	p[0].position.y   = -SKY_EXTENTS;
	p[0].position.z   = -SKY_EXTENTS;
	p[0].tu = 0;
	p[0].tv = 0;
	

	p[1].position.x   = -SKY_EXTENTS;
	p[1].position.y   = SKY_EXTENTS;
	p[1].position.z   = -SKY_EXTENTS;
	p[1].tu = 0;
	p[1].tv = 1;
	
	

	p[2].position.x   = -SKY_EXTENTS;
	p[2].position.y   = -SKY_EXTENTS;
	p[2].position.z   = SKY_EXTENTS;
	p[2].tu = 1;
	p[2].tv = 0;
		

	p[3].position.x   = -SKY_EXTENTS;
	p[3].position.y   = SKY_EXTENTS;
	p[3].position.z   = SKY_EXTENTS;
	p[3].tu = 1;
	p[3].tv = 1;
	
	if(West > -1 )
	{
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(ParticleVertex));		

		//Draw the fog overlay
		/*Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((int)r, (int)g, (int)b) );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Screen.SetTexture( BlendTexture, 0 );
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CustomVertex));
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Screen.SetMaterial( NULL);
		Screen.SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255) );*/
	}
	

	
	Screen.SetRenderState(D3DRS_LIGHTING, TRUE);
	Screen.SetRenderState(D3DRS_ZENABLE, TRUE);
	Screen.SetRenderState(D3DRS_FOGENABLE, TRUE);
}