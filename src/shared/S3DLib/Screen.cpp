#include "screen.h"
#include "vertexbuffer.h"
#include "VertexShader.h"
#include "Font.h"


//#define NVPERFHUD

//GLOBALS//////////////////////////////////////////////////////////////
extern int redbits, greenbits, bluebits, rgbbits; //Work well as globals


void DisplayModeInfo::PrintInfo( char* outstring )
{
	
	sprintf(outstring, "%dx%d (%d BPP) @ %dHz", Width, Height, BPP, RefreshRate );
}

Screen3D::Screen3D() 
{  
	D3D		  = NULL; 
	D3DDevice = NULL; 
	FOV		  = 75;  
    m_wireframe = false;
	DrawCalls = 0;

	InitializeCriticalSection( &screenCriticalSection );
}

/*

int Screen3D::GetModeListSize()
{
	if( NULL ==  D3D )
		D3D = Direct3DCreate8( D3D_SDK_VERSION );

	return D3D->GetAdapterModeCount( 0 );
}

DisplayModeInfo Screen3D::GetModeListIndex( int index)
{
	if( NULL ==  D3D )
		D3D = Direct3DCreate8( D3D_SDK_VERSION );

	//Declarations
	DisplayModeInfo mode;
	D3DDISPLAYMODE d3dmode;

	//Get the display mode
	D3D->EnumAdapterModes( 0, index, &d3dmode );

	//Set the mode data
	mode.Width  = d3dmode.Width;
	mode.Height = d3dmode.Height;
	mode.RefreshRate = d3dmode.RefreshRate;
	mode.Fullscreen  = true;
	mode.BackBufferFormat = d3dmode.Format;
	
	//setup the BPP
	if(d3dmode.Format == D3DFMT_R8G8B8)
		mode.BPP    = 24;
	else if(d3dmode.Format == D3DFMT_X8R8G8B8 || d3dmode.Format == D3DFMT_A8R8G8B8)
		mode.BPP    = 32;
	else if(d3dmode.Format == D3DFMT_R5G6B5 )
		mode.BPP    = 16;
	else if(d3dmode.Format == D3DFMT_X1R5G5B5 || d3dmode.Format == D3DFMT_A1R5G5B5 )
		mode.BPP    = 15;
	

	//Return the mode!
	return mode;
}

bool Screen3D::ModeExists( DisplayModeInfo &DM )
{
	//declarations
	int size = GetModeListSize(); //List size
	
	//Loop through all the modes and find the closest match
	DisplayModeInfo rval;
	for(int i=0; i<size; i++)
	{
		//Get the mode
		rval = GetModeListIndex( i );
		
		//check if it matches the one passed
		if( rval.Width  == DM.Width &&
			rval.Height == DM.Height &&
			rval.BPP    == DM.BPP && 
			rval.BackBufferFormat == DM.BackBufferFormat)
		{
			return true;
		}
	}
	return false;
}

int Screen3D::FindClosestMode( DisplayModeInfo &DM )
{
	if( NULL ==  D3D )
		D3D = Direct3DCreate8( D3D_SDK_VERSION );

	//declarations
	int size = GetModeListSize(); //List size
	int difference=1000000;       //difference between modes. Difference of 0 means it's an exact match.
	int currentmode=0;            //current mode found that's closest
	int newdifference=0;		  //temporarily stores difference calculation

	//Loop through all the modes and find the closest match
	DisplayModeInfo rval;
	for(int i=0; i<size; i++)
	{
		//Get the mode
		rval = GetModeListIndex( i );
		
		//calculate the difference between the desired mode (DM), and the
		//mode we just got.
		int newdifference = abs(DM.Width - rval.Width) + abs( DM.Height - rval.Height ) + abs( DM.BPP - rval.BPP );

		//Difference is zero! We found exactly what we want!
		if(newdifference == 0)
			return i;
        
		//If this is closer than the last value we chose, set it.
		if(newdifference < difference)
		{
			currentmode = i;
			difference  = newdifference;
		}
	}

	//Return the closest mode we could find
	return currentmode;
}

*/


bool Screen3D::Create(HWND hwnd, DisplayModeInfo& m, float viewdistance)
{

	if( NULL == ( D3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	    return false;


	D3DDISPLAYMODE d3ddm;
	if( FAILED( D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
		return false;
	
	hWnd = hwnd;

	
	//Get the client rectangle dimensions  of the window.
	RECT WindowRect;
	GetClientRect(hwnd, &WindowRect);

	if( m.Width == -1 || m.Height == -1)
	{
		DisplayMode.Width  = WindowRect.right - WindowRect.left;
		DisplayMode.Height = WindowRect.bottom - WindowRect.top;
	}
	else
	{
		DisplayMode.Width  = m.Width;
		DisplayMode.Height = m.Height;
	}

	OutputMessage( "Attempting Resolution: %dx%d\n", DisplayMode.Width, DisplayMode.Height);

    // Set up the structure used to create the D3DDevice. 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
	if ( DisplayMode.Fullscreen )
	{
		d3dpp.Windowed   = FALSE; 
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //D3DSWAPEFFECT_COPY ;
		d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_ONE  ;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT ;
	}
	else
	{
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.Windowed = TRUE; 
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	}
    
	d3dpp.BackBufferWidth  = DisplayMode.Width ;
	d3dpp.BackBufferHeight = DisplayMode.Height;
    d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	
	OutputMessage( "BackBuffer: %d\n", d3ddm.Format );

	//Get the depth/stencil caps

	//check for 32bit depth buffer
	if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D32) ))
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
	}

	//second choice is 24bit buffer with 8 bit stencil
	else if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D24S8) ))
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	//third  choice is 24bit buffer with 4 bit stencil
	else if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D24X4S4) ))
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
	}

	//fourth choice is 24bit buffer with 8 bits wasted
	else if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D24X8) ))
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	}

	//fifth choice is 16bit buffer
	else if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D16) ) )
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}

	//last choice is 15bit depth buffer with 1 bit stencil
	else if(  SUCCEEDED ( D3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_SURFACE, D3DFMT_D15S1) ) )
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D15S1;
	}

	OutputMessage( "DepthStencil: %d\n", d3dpp.AutoDepthStencilFormat );
	

    // Create the D3DDevice
	HRESULT hr;
	DWORD VertexFlags=0;
	if(true)
		VertexFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		VertexFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;


	// Set default settings
	//UINT AdapterToUse=D3DADAPTER_DEFAULT; //D3D->GetAdapterCount()-1
	//D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL; //D3DDEVTYPE_REF
#ifdef NVPERFHUD
	UINT AdapterToUse=D3D->GetAdapterCount()-1;
	D3DDEVTYPE DeviceType=D3DDEVTYPE_REF;
#else
	UINT AdapterToUse=D3DADAPTER_DEFAULT; //D3D->GetAdapterCount()-1
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL; //D3DDEVTYPE_REF
#endif

	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter=0;Adapter<D3D->GetAdapterCount();Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res;
		Res = D3D->GetAdapterIdentifier(Adapter,0,&Identifier);
		if (strcmp(Identifier.Description,"NVIDIA NVPerfHUD") == 0)
		{
			AdapterToUse=Adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}


    if( FAILED( hr = D3D->CreateDevice(
								AdapterToUse,//D3DADAPTER_DEFAULT,
								DeviceType,//D3DDEVTYPE_HAL, 
								hwnd,
								VertexFlags ,
								&d3dpp, 
								&D3DDevice 
								)
		) )
    {
		OutputDXError( hr, "D3D Device Init Failed ");
		return false;
    }
	else
	{

		//We have a mixed device, so figure out the level of T&L support
		D3DCAPS9 tlcaps;
		D3DDevice->GetDeviceCaps(&tlcaps);       // initialize m_pd3dDevice before using

		//check the shader version
		if(	D3DSHADER_VERSION_MAJOR( tlcaps.VertexShaderVersion ) < 1 ) 
			TLSupport = S3D_STANDARD;

		//Major version is 1, so check minor versions
		else if(D3DSHADER_VERSION_MINOR( tlcaps.VertexShaderVersion ) < 1 ) 
			TLSupport = S3D_VS_1_0; //Vertex shader 1.0
		else
			TLSupport = S3D_VS_1_1; //Vertex shader 1.1 or above
	}


	//set up the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, RADIAN(60), (float)DisplayMode.Width/(float)DisplayMode.Height, 1.0f, viewdistance );
    D3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	//figure out the field of view
	CalculateFOV();


    // initial render state
    D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    D3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE  );

	D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	SetLinearTextureFiltering();
    	
	//init the texture manager
	TM.Init(this);
	TM.RestoreTextures();

	InitialAvailableMem = D3DDevice->GetAvailableTextureMem();

	//Init the font manager
	FM.Init( *this );
	//return OK
	return true;

}

void Screen3D::Destroy()
{

	FM.Cleanup();
	TM.Destroy();

    if( D3DDevice != NULL )
        D3DDevice->Release();

    if( D3D != NULL )
        D3D->Release();

	D3DDevice = NULL;
	D3D = NULL;
	

}


int Screen3D::GetDeviceCapability ( DeviceCaps CapToCheck )
{
	D3DCAPS9 Caps;
	D3DDevice->GetDeviceCaps(&Caps);

	if( CapToCheck == S3DDev_Hardware)
	{
		return Caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION;
	}

	if( CapToCheck == S3DDev_PureDevice)
	{
		return Caps.DevCaps & D3DDEVCAPS_PUREDEVICE;
	}
	
	if( CapToCheck == S3DDev_MaxLights)
	{
		return Caps.MaxActiveLights;
	}

	if( CapToCheck == S3DDev_MaxTextureWidth)
	{
		return Caps.MaxTextureWidth;
	}

	if( CapToCheck == S3DDev_MaxTextureHeight)
	{
		return Caps.MaxTextureHeight;
	}

		if( CapToCheck == S3DDev_MaxTextureStages)
	{
		return Caps.MaxSimultaneousTextures;
	}

	if( CapToCheck == S3DDev_MaxPointSize )
		return Caps.MaxPointSize;

	return -1;
}


bool Screen3D::ChangeViewDepth(Real ViewDepth)
{
	ScreenViewDepth = ViewDepth;
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, RADIAN(FOV),((float)GetWidth()/(float)GetHeight()), 0.2f, ViewDepth );
	D3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	return true;
}

void Screen3D::SetFieldOfView( int newFOV )
{
	FOV = newFOV;

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, RADIAN(FOV),( (float)GetWidth()/(float)GetHeight()), 0.2f, ScreenViewDepth );
	D3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//extract the frustum planes
//multiply the projection matrix by 
//the view matrix
void Screen3D::CalculateFOV()
{
	D3DXMATRIX ViewMatrix,ProjMatrix,ViewProj;

	D3DDevice->GetTransform(D3DTS_PROJECTION,&ProjMatrix);
	D3DDevice->GetTransform(D3DTS_VIEW,&ViewMatrix);

	D3DXMatrixMultiply(&ViewProj,&ViewMatrix,&ProjMatrix); //Combine Them

	// Left clipping plane
	FrustumPlanes[0].Normal.x = (ViewProj._14 + ViewProj._11);
	FrustumPlanes[0].Normal.y = (ViewProj._24 + ViewProj._21);
	FrustumPlanes[0].Normal.z = (ViewProj._34 + ViewProj._31);
	FrustumPlanes[0].Distance = (ViewProj._44 + ViewProj._41);

	// Right clipping plane
	FrustumPlanes[1].Normal.x = (ViewProj._14 - ViewProj._11);
	FrustumPlanes[1].Normal.y = (ViewProj._24 - ViewProj._21);
	FrustumPlanes[1].Normal.z = (ViewProj._34 - ViewProj._31);
	FrustumPlanes[1].Distance = (ViewProj._44 - ViewProj._41);

	// Top clipping plane
	FrustumPlanes[2].Normal.x = (ViewProj._14 - ViewProj._12);
	FrustumPlanes[2].Normal.y = (ViewProj._24 - ViewProj._22);
	FrustumPlanes[2].Normal.z = (ViewProj._34 - ViewProj._32);
	FrustumPlanes[2].Distance = (ViewProj._44 - ViewProj._42);

	// Bottom clipping plane
	FrustumPlanes[3].Normal.x = (ViewProj._14 + ViewProj._12);
	FrustumPlanes[3].Normal.y = (ViewProj._24 + ViewProj._22);
	FrustumPlanes[3].Normal.z = (ViewProj._34 + ViewProj._32);
	FrustumPlanes[3].Distance = (ViewProj._44 + ViewProj._42);

	// Near clipping plane
	FrustumPlanes[4].Normal.x = (ViewProj._14 + ViewProj._13);
	FrustumPlanes[4].Normal.y = (ViewProj._24 + ViewProj._23);
	FrustumPlanes[4].Normal.z = (ViewProj._34 + ViewProj._33);
	FrustumPlanes[4].Distance = (ViewProj._44 + ViewProj._43);

	// Far clipping plane
	FrustumPlanes[5].Normal.x = (ViewProj._14 - ViewProj._13);
	FrustumPlanes[5].Normal.y = (ViewProj._24 - ViewProj._23);
	FrustumPlanes[5].Normal.z = (ViewProj._34 - ViewProj._33);
	FrustumPlanes[5].Distance = (ViewProj._44 - ViewProj._43);


	// NORMALIZE THE PLANES 
	//needed for sphere checks since we need radius and distances to be correct

	for (int i=0;i<6;i++)
	{
		Real denom = 1.0f / D3DXVec3Length(&FrustumPlanes[i].Normal);// Get magnitude of Vector
		FrustumPlanes[i].Normal.x *= denom;
		FrustumPlanes[i].Normal.y *= denom;
		FrustumPlanes[i].Normal.z *= denom;
		FrustumPlanes[i].Distance *= denom;
	}

}

//return true if point is visible
bool Screen3D::CheckPointVisibility(Point3D& Point)
{
	for(int p = 0; p < 6; p++ )
	   if( FrustumPlanes[p].Normal.x * Point.x + FrustumPlanes[p].Normal.y * Point.y + FrustumPlanes[p].Normal.z * Point.z + FrustumPlanes[p].Distance <= 0 )
	       return false;
	return true;
}

//not completed yet
int Screen3D::BoxInFrustum(BoundingBox& BBox)
{
	return 1;
}

//return 0 if not at all in frustum
//return 1 if partialy
//return 2 if fully
int Screen3D::SphereInFrustum(BoundingSphere& BSphere)
{
	Real d;
	int c=0;
	for(int p = 0; p < 6; p++ )
	{
		d = FrustumPlanes[p].Normal.x * BSphere.x() + FrustumPlanes[p].Normal.y * BSphere.y() + FrustumPlanes[p].Normal.z * BSphere.z() + FrustumPlanes[p].Distance;
		if(d <= -BSphere.Radius)
			return 0;
		if(d >BSphere.Radius)
			c++;
	}
	return (c == 6) ? 2 : 1;
}


void Screen3D::GetMouseRay(int x, int y, Vector3D* OutRayOrigin, Vector3D* OutRayDir )
{
	D3DXMATRIX matProj;
    D3DDevice->GetTransform( D3DTS_PROJECTION, &matProj );


    // Compute the vector of the pick ray in screen space
    D3DXVECTOR3 v;
    v.x =  ( ( ( 2.0f * x ) / GetWidth()  ) - 1 ) / matProj._11;
    v.y = -( ( ( 2.0f * y ) / GetHeight() ) - 1 ) / matProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX matView, m;
    D3DDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &m, NULL, &matView );

    // Transform the screen space pick ray into 3D space
    OutRayDir->x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    OutRayDir->y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    OutRayDir->z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
    OutRayOrigin->x = m._41;
    OutRayOrigin->y = m._42;
    OutRayOrigin->z = m._43;
}


void Screen3D::GetScreenCoordinates( Vector3D* Out, Vector3D& Input )
{

	//Declarations
	D3DXVECTOR3 out,  in;
	D3DVIEWPORT9 viewport;
	D3DXMATRIX mView, mWorld, mProjection, mInvertedView ;


	//Get the variables we need
	in = Input;
	D3DDevice->GetViewport( &viewport);
	D3DDevice->GetTransform( D3DTS_VIEW, &mView );
	D3DDevice->GetTransform( D3DTS_WORLD, &mWorld );
	D3DDevice->GetTransform( D3DTS_PROJECTION, &mProjection );

	
	//project the vector
	D3DXVec3Project(&out, &in, &viewport, &mProjection, &mView, &mWorld );

	//done! We now have the projected (screen) coordinates
	*Out = out;
}


//return 0 if not at all in frustum
//return 1 if partialy
//return 2 if fully
int Screen3D::CheckAABB(AABB& AABBox)
{
	AABB MinMaxExtremes;
	bool intersect = false;
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)
		{
			if (FrustumPlanes[i].Normal[j] < 0.0f)
			{
				MinMaxExtremes.min[j] = AABBox.min[j];
				MinMaxExtremes.max[j] = AABBox.max[j];
			}
			else{
				MinMaxExtremes.min[j] = AABBox.max[j];
				MinMaxExtremes.max[j] = AABBox.min[j];
			}
		}
		if(FrustumPlanes[i].DistanceToPoint(MinMaxExtremes.min)<0.0f)return 0;
		if(FrustumPlanes[i].DistanceToPoint(MinMaxExtremes.max)<=0.0f)intersect = true;
	}
	if(intersect)return 1;
	return 2;
}

//return 0 if not at all in frustum
//return 1 if partialy
//return 2 if fully
int Screen3D::CubeInFrustum(BoundingCube& BCube)
{
	int p;
	int c;
	int c2 = 0;
	for( p = 0; p < 6; p++ )
	{
		c = 0;
		if(FrustumPlanes[p].Normal.x * (BCube.x - BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y - BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z - BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x + BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y - BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z - BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x - BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y + BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z - BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x + BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y + BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z - BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x - BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y - BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z + BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x + BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y - BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z + BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x - BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y + BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z + BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if(FrustumPlanes[p].Normal.x * (BCube.x + BCube.Size)+ FrustumPlanes[p].Normal.y * (BCube.y + BCube.Size) + FrustumPlanes[p].Normal.z * (BCube.z + BCube.Size) + FrustumPlanes[p].Distance > 0 )
			c++;
		if( c == 0 )
			return 0;
		if( c == 8 )
			c2++;
	}   
	return (c2 == 6) ? 2 : 1;
}


void Screen3D::SetViewport(int x, int y, int width, int height)
{
	if(!D3DDevice)
		return;

	D3DVIEWPORT9 NewViewport;

	//Width = width;
	//Height = height;
	D3DDISPLAYMODE mode;
	D3DDevice->GetDisplayMode( 0, &mode);

	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	if(x >= GetWidth())
		x = GetWidth() - 1;
	if(y >= GetHeight())
		y = GetHeight() - 1;

	if(x+width > GetWidth())
		width = GetWidth()-x;

	if(y+height > GetHeight())
		height = GetHeight()-y;

	NewViewport.X = x;
	NewViewport.Y = y;
	NewViewport.Width = width;
	NewViewport.Height = height;
	NewViewport.MinZ   = 0;
	NewViewport.MaxZ   = 1;


	D3DDevice->SetViewport(&NewViewport);
}


void Screen3D::Start3D() {
	if(!D3DDevice)
		return;

	D3DDevice->BeginScene();
}

void Screen3D::End3D() {
	if(!D3DDevice)
		return;

	FlushOverlays();

	D3DDevice->EndScene();
}

void Screen3D::PostOverlay( int x, int y, TextureHandle image, float alpha )
{
	OverlayQueue.push_back( Overlay( image, alpha, x, y ) );
}


void Screen3D::FlushOverlays()
{
	SetTextureMode(TMNormal);
	
	SetRenderState(D3DRS_FOGENABLE, FALSE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	std::list<Overlay>::iterator it;

	for( it = OverlayQueue.begin(); it != OverlayQueue.end(); ++it )
	{
		this->DrawSprite( (*it).x, (*it).y, (*it).Image, ((*it).Alpha)*255.0f );
	}

	OverlayQueue.clear();
}

void Screen3D::Clear(bool ZOnly)
{
	
	if(D3DDevice)
	{
		DWORD Flags=0;

		Flags |= D3DCLEAR_ZBUFFER;

		if(!ZOnly)
			Flags |= D3DCLEAR_TARGET;
		
		D3DDevice->Clear(NULL, NULL, Flags, D3DCOLOR_ARGB(A, R, G, B), 1.0f, 0);
	}
}

void Screen3D::UpdateScreen(bool StretchToFit)
{
	//declarations
	HRESULT HR;

	if(D3DDevice)
	{
		RECT ClientRect;
		GetClientRect(hWnd, &ClientRect);
		
		//update the screen
		if(!StretchToFit)
			HR = D3DDevice->Present( &ClientRect, &ClientRect, NULL, NULL );
		else
			HR = D3DDevice->Present( NULL, NULL, NULL, NULL );

		//check for a lost device			
		if( HR == D3DERR_DEVICELOST )
		{
			//delete any resources that were allocated
			TM.InvalidateRenderTargets();

			while(D3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			{
				D3DDevice->Reset(&d3dpp);
				Sleep(1);
			}
			TM.RestoreRenderTargets();
			
		}

	}
	PolygonsRendered = 0;
	DrawCalls = 0;

}

void Screen3D::ShowText(int x, int y, char string[512])
{
      //!
}


TextureHandle Screen3D::GetBlankTexture( const char* name, const char* alphamask, int width, int height)
{
	TextureHandle i = TM.TextureExists(name);

	if(i == -1)
	{
		return TM.AddTexture(name, alphamask, width, height, true);
	}

	return i;

}

TextureHandle Screen3D::GetTextureHandle(const char* filename, const char* alphamask)
{
	TextureHandle i = TM.TextureExists(filename);

	if(i == -1)
	{
		return TM.AddTexture(filename, alphamask);
	}

	return i;

}


void Screen3D::SetTexture(Texture* texture, int stage) 
{
	if(!D3DDevice)
		return ;

	if(texture != NULL)
	{
		D3DDevice->SetTexture(stage, texture->TextureSurface);
	}
	else
	{
		D3DDevice->SetTexture(stage, NULL);
	}
}


void Screen3D::SetTexture(TextureHandle h, int Stage)
{
	SetTexture(TM.GetTexture(h), Stage);
}

void Screen3D::Wireframe(bool on_or_off)
{
	if(on_or_off )
		D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
}



//texture filtering
void Screen3D::SetPointTextureFiltering(int stage, int endstage)
{
	SetTextureFiltering( D3DTEXF_POINT, stage, endstage );

	/*if( endstage == -1)
		endstage = stage;
	for( int i=stage; i<=endstage; i++)
		D3DDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);*/

}

void Screen3D::SetLinearTextureFiltering(int stage, int endstage)
{
	SetTextureFiltering( D3DTEXF_LINEAR, stage, endstage );
}

void Screen3D::SetAnisotropicTextureFiltering( int stage, int endstage)
{
	SetTextureFiltering( D3DTEXF_ANISOTROPIC, stage, endstage );
}

void Screen3D::SetTextureFiltering( DWORD filter, int stage, int endstage )
{
	if( endstage == -1)
		endstage = stage;
	for( int i=stage; i<=endstage; i++)
	{
		D3DDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, filter);
		D3DDevice->SetSamplerState(i, D3DSAMP_MINFILTER, filter);
		D3DDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, filter);
	}
}


void Screen3D::SetStreamSource( VertexBuffer* vb, int strm) 
{
	if( vb )
		vb->SetActive( strm );
	else
		D3DDevice->SetStreamSource(strm , 0, 0, 0 );
}


//vertex shader
void Screen3D::SetVertexShader( IDirect3DVertexShader9* vs ) { 
	D3DDevice->SetVertexShader( vs );
}


void Screen3D::SetVertexShaderConstant( int reg, const D3DXVECTOR4* constant, int count) {
	D3DDevice->SetVertexShaderConstantF( reg, (const float*) constant, count );
}

void Screen3D::SetVertexShaderConstant( int reg, const D3DXMATRIX* constant) {
	D3DDevice->SetVertexShaderConstantF( reg, (const float*) constant, 4);
}



void Screen3D::DefaultMaterial()
{
	// Set up a material. 
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	D3DDevice->SetMaterial( &mtrl );
}

void Screen3D::SetMaterial(D3DMATERIAL9* Mat)
{
	if(!D3DDevice)
		return;

	if(Mat == NULL)
	{
		DefaultMaterial();
		return;
	}


	D3DDevice->SetMaterial( Mat);
}



void Screen3D::SetTransform ( Matrix& M ) {
	if(!D3DDevice)
		return;

	D3DDevice->SetTransform( D3DTS_WORLDMATRIX(0), &M.Mat);
}

void Screen3D::SetTransform ( D3DXMATRIX& M ) {
	if(!D3DDevice)
		return;
	D3DDevice->SetTransform( D3DTS_WORLDMATRIX(0), &M);
}

void Screen3D::MultTransform ( Matrix& M ) {
	if(!D3DDevice)
		return;
	D3DDevice->MultiplyTransform( D3DTS_WORLDMATRIX(0), &M.Mat);
}

void Screen3D::SetViewTransform ( Matrix& M, bool CalcFOV ) {
	if(!D3DDevice)
		return;
	
	D3DXMATRIX Inverse;
	Real Determinant;

	Inverse = M.Mat;
	D3DXMatrixInverse(&Inverse, &Determinant, &M.Mat);

	D3DDevice->SetTransform( D3DTS_VIEW, &M.Mat);

	if(CalcFOV)
		CalculateFOV();//calculate the new frustum planes

	
	
}

void Screen3D::SetProjectionMatrix( Matrix& M )
{
	D3DDevice->SetTransform( D3DTS_PROJECTION, &M.Mat );
}



void Screen3D::TakeScreenshot(char* basefilename)
{
	//declarations
	IDirect3DSurface9* FrontBuffer; //this is our pointer to the memory location containing our copy of the
                                //front buffer
	IDirect3DSurface9* Thumbnail;
	HRESULT hr;

	//Create surface for stuff to be copied to
	
	//D3DDevice->CreateOffscreenPlainSurface(DisplayMode.Width, DisplayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &FrontBuffer, NULL);
	hr = D3DDevice->CreateOffscreenPlainSurface(100, 75, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH , &Thumbnail, NULL);

	//now we copy the front buffer into our surface
	//HRESULT hr = D3DDevice->GetFrontBufferData(0, FrontBuffer);
	hr = D3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &FrontBuffer);

	

	//make sure we got the front buffer
	if(hr != D3D_OK)
	{
		Thumbnail->Release(); 
		FrontBuffer->Release(); 
		return;
	}

	//load the image into the thumbnail
	D3DXLoadSurfaceFromSurface(Thumbnail, NULL, NULL, FrontBuffer, NULL, NULL, D3DX_DEFAULT, 0);

	//write screen shot to a bitmap file, as well as a thumbnail
	std::string BmpFileName = "..\\screenshots\\";
	std::string ThumbFileName = "..\\screenshots\\_";
	BmpFileName += basefilename;
	BmpFileName += ".bmp";
	ThumbFileName += basefilename;
	ThumbFileName += ".bmp";
	D3DXSaveSurfaceToFile(BmpFileName.c_str(), D3DXIFF_BMP, FrontBuffer, NULL, NULL);
	D3DXSaveSurfaceToFile(ThumbFileName.c_str(), D3DXIFF_BMP, Thumbnail, NULL, NULL);

	//release the surface so there is no memory leak
	FrontBuffer->Release();
	Thumbnail->Release();

}


//Draws a Vertex Buffer set with SetVertexSource. Fails if the vertex
//source is null. You can optionally also provide an IndexBuffer
void Screen3D::DrawPrimitiveVB ( VertexBuffer* VB, IndexBuffer* IB, _D3DPRIMITIVETYPE PrimType, DWORD VertexShader,  int Offset, int Count )
{
	HRESULT HR;
	VB->SetActive(0 );
	HR = D3DDevice->SetFVF( VB->FVF() );

	if(!Count)
	{
		if( IB ) 
			Count = IB->GetIndexCount();
		else
			Count = VB->VertexCount();

		if(PrimType == D3DPT_TRIANGLELIST)
			Count /= 3;
		else if(PrimType == D3DPT_TRIANGLESTRIP)
			Count -=2;
		else if(PrimType == D3DPT_TRIANGLEFAN)
			Count -=2;
		else if(PrimType == D3DPT_LINESTRIP)
			Count -=2;
		else if(PrimType == D3DPT_LINELIST)
			Count /= 2;
	}

	PolygonsRendered += Count;
	DrawCalls++;

	//check to see if we're using an index buffer with this
	if(IB)
	{
		//draw the primitive	
		HR = D3DDevice->SetIndices(IB->GetD3DBufferInterface());
		HR = D3DDevice->DrawIndexedPrimitive(PrimType, 0, 0, VB->VertexCount(), Offset*3, Count - Offset);

	}
	else
	{		
		//draw the primitive
		HR = D3DDevice->DrawPrimitive(PrimType, Offset*3, Count - Offset);
		
	}

	D3DDevice->SetStreamSource(0, NULL, 0, 0);
}


void Screen3D::EnableWireframeDrawing()
{
	m_wireframe = true;
	D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
}

void Screen3D::EnableSolidDrawing()
{
	m_wireframe = false;
	D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
}

void Screen3D::ResetWireframeState()
{
	if( m_wireframe )
		D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );

}


void Screen3D::BlankArea( RECT& Dimensions)
{

	// Figure out the vertices for the main window
	ScreenVertex WindowVertices[4];

	WindowVertices[0].z   = 0;WindowVertices[1].z   = 0;WindowVertices[2].z   = 0;WindowVertices[3].z   = 0;
	WindowVertices[0].RHW = 1;	WindowVertices[1].RHW = 1; WindowVertices[2].RHW = 1; WindowVertices[3].RHW = 1;
	WindowVertices[0].color = D3DCOLOR_RGBA(R, G, B, 255);WindowVertices[1].color = D3DCOLOR_RGBA(R, G, B, 255);
	WindowVertices[2].color = D3DCOLOR_RGBA(R, G, B, 255);WindowVertices[3].color = D3DCOLOR_RGBA(R, G, B, 255);

	WindowVertices[0].x   = Dimensions.left;
	WindowVertices[0].y   = Dimensions.top;

	
	WindowVertices[1].x   = Dimensions.right;
	WindowVertices[1].y   = Dimensions.top;

	WindowVertices[2].x   = Dimensions.left;
	WindowVertices[2].y   = Dimensions.bottom;

	WindowVertices[3].x   = Dimensions.right;
	WindowVertices[3].y   = Dimensions.bottom;

	PolygonsRendered += 2;
	DrawCalls++;

	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);
	D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	SetFVF(D3DFVF_SCREENVERTEX);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, WindowVertices,  sizeof(ScreenVertex));

}

void Screen3D::DrawSpriteRect( RECT& Dimensions, TextureHandle Image, bool StretchTexture, D3DCOLOR color)
{
	//Figure out the size of the texture (if there is a texture)
	SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	Texture* T = TM.GetTexture(Image);
	Real texturewidth=1, textureheight=1;
	if(T)
	{
		texturewidth  = T->GetWidth();
		textureheight = T->GetHeight();
	}


	// Figure out the vertices for the main window
	ScreenVertex WindowVertices[4];

	WindowVertices[0].z   = 0;WindowVertices[1].z   = 0;WindowVertices[2].z   = 0;WindowVertices[3].z   = 0;
	WindowVertices[0].RHW = 1;	WindowVertices[1].RHW = 1; WindowVertices[2].RHW = 1; WindowVertices[3].RHW = 1;
	WindowVertices[0].color = color;WindowVertices[1].color = color;
	WindowVertices[2].color = color;WindowVertices[3].color = color;

	WindowVertices[0].x   = Dimensions.left;
	WindowVertices[0].y   = Dimensions.top;

	
	WindowVertices[1].x   = Dimensions.right;
	WindowVertices[1].y   = Dimensions.top;

	WindowVertices[2].x   = Dimensions.left;
	WindowVertices[2].y   = Dimensions.bottom;

	WindowVertices[3].x   = Dimensions.right;
	WindowVertices[3].y   = Dimensions.bottom;


	if( StretchTexture )
	{
		WindowVertices[0].tu  = 0;
		WindowVertices[0].tv  = 0;
		WindowVertices[1].tu  = 1;
		WindowVertices[1].tv  = 0;
		WindowVertices[2].tu  = 0;
		WindowVertices[2].tv  = 1;
		WindowVertices[3].tu  = 1;
		WindowVertices[3].tv  = 1;
	}
	else
	{
		WindowVertices[0].tu  = 0;
		WindowVertices[0].tv  = 0;
		WindowVertices[1].tu  = (Dimensions.right-Dimensions.left) / texturewidth;
		WindowVertices[1].tv  = 0;
		WindowVertices[2].tu  = 0;
		WindowVertices[2].tv  = (Dimensions.bottom-Dimensions.top) / textureheight;
		WindowVertices[3].tu  = (Dimensions.right-Dimensions.left) / texturewidth;
		WindowVertices[3].tv  = (Dimensions.bottom-Dimensions.top) / textureheight;
	}

	PolygonsRendered += 2;
	DrawCalls++;

	SetTexture(Image, 0);
	/*D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);*/
	SetFVF(D3DFVF_SCREENVERTEX);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, WindowVertices,  sizeof(ScreenVertex));

	ResetWireframeState();

}



void Screen3D::DrawSpriteNoHandle( RECT& Dimensions, Texture* Image, bool StretchTexture, D3DCOLOR color, bool SetAlphaStates)
{
	SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	//Figure out the size of the texture (if there is a texture)
	Texture* T = Image;
	Real texturewidth=1, textureheight=1;
	if(T)
	{
		texturewidth  = T->GetWidth();
		textureheight = T->GetHeight();
	}


	// Figure out the vertices for the main window
	ScreenVertex WindowVertices[4];

	WindowVertices[0].z   = 0;WindowVertices[1].z   = 0;WindowVertices[2].z   = 0;WindowVertices[3].z   = 0;
	WindowVertices[0].RHW = 1;	WindowVertices[1].RHW = 1; WindowVertices[2].RHW = 1; WindowVertices[3].RHW = 1;
	WindowVertices[0].color = color;WindowVertices[1].color = color;
	WindowVertices[2].color = color;WindowVertices[3].color = color;

	WindowVertices[0].x   = Dimensions.left;
	WindowVertices[0].y   = Dimensions.top;

	
	WindowVertices[1].x   = Dimensions.right;
	WindowVertices[1].y   = Dimensions.top;

	WindowVertices[2].x   = Dimensions.left;
	WindowVertices[2].y   = Dimensions.bottom;

	WindowVertices[3].x   = Dimensions.right;
	WindowVertices[3].y   = Dimensions.bottom;


	if( StretchTexture )
	{
		WindowVertices[0].tu  = 0;
		WindowVertices[0].tv  = 0;
		WindowVertices[1].tu  = 1;
		WindowVertices[1].tv  = 0;
		WindowVertices[2].tu  = 0;
		WindowVertices[2].tv  = 1;
		WindowVertices[3].tu  = 1;
		WindowVertices[3].tv  = 1;
	}
	else
	{
		WindowVertices[0].tu  = 0;
		WindowVertices[0].tv  = 0;
		WindowVertices[1].tu  = (Dimensions.right-Dimensions.left) / texturewidth;
		WindowVertices[1].tv  = 0;
		WindowVertices[2].tu  = 0;
		WindowVertices[2].tv  = (Dimensions.bottom-Dimensions.top) / textureheight;
		WindowVertices[3].tu  = (Dimensions.right-Dimensions.left) / texturewidth;
		WindowVertices[3].tv  = (Dimensions.bottom-Dimensions.top) / textureheight;
	}

	PolygonsRendered += 2;
	DrawCalls++;

	SetTexture(Image, 0);
	if(SetAlphaStates)
	{
		D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
		D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	}
	SetFVF(D3DFVF_SCREENVERTEX);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, WindowVertices,  sizeof(ScreenVertex));

}

void Screen3D::DrawSpritePoints( Point3D* pointlist, TextureHandle Image )
{

	SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
	// Figure out the vertices for the main window
	ScreenVertex WindowVertices[4];

	WindowVertices[0].z   = 0;WindowVertices[1].z   = 0;WindowVertices[2].z   = 0;WindowVertices[3].z   = 0;
	WindowVertices[0].RHW = 1;	WindowVertices[1].RHW = 1; WindowVertices[2].RHW = 1; WindowVertices[3].RHW = 1;
	WindowVertices[0].color = D3DCOLOR_RGBA(255, 255, 255, 255);WindowVertices[1].color = D3DCOLOR_RGBA(255, 255, 255, 255);
	WindowVertices[2].color = D3DCOLOR_RGBA(255, 255, 255, 255);WindowVertices[3].color = D3DCOLOR_RGBA(255, 255, 255, 255);

	WindowVertices[0].x   = pointlist[0].x;
	WindowVertices[0].y   = pointlist[0].y;
	WindowVertices[1].x   = pointlist[1].x;
	WindowVertices[1].y   = pointlist[1].y;
	WindowVertices[2].x   = pointlist[2].x;
	WindowVertices[2].y   = pointlist[2].y;
	WindowVertices[3].x   = pointlist[3].x;
	WindowVertices[3].y   = pointlist[3].y;



	WindowVertices[0].tu  = 0;
	WindowVertices[0].tv  = 0;
	WindowVertices[1].tu  = 1;
	WindowVertices[1].tv  = 0;
	WindowVertices[2].tu  = 0;
	WindowVertices[2].tv  = 1;
	WindowVertices[3].tu  = 1;
	WindowVertices[3].tv  = 1;


	SetTexture(Image, 0);
	SetFVF(D3DFVF_SCREENVERTEX);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, WindowVertices,  sizeof(ScreenVertex));

	ResetWireframeState	();
}

void Screen3D::DrawSprite( int x, int y, Texture* Image, int alpha)
{
	SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );

	Texture* T = Image;
	Real texturewidth=1, textureheight=1;
	if(T)
	{
		texturewidth  = T->GetWidth();
		textureheight = T->GetHeight();
	}

	//float jiggle= (rand() % 10000) - 5000;
	//jiggle /= 5000.0f;
	float jiggle = 0.5f;

	if( alpha != 255 )
		jiggle = 0;
	x--;
	y--;
	

	ScreenVertex v[6];
	D3DCOLOR color = D3DCOLOR_ARGB( alpha, 255, 255, 255 );
	v[0] = ScreenVertex( x+jiggle,   y+jiggle,   1,					color, 0, 0 );
	v[2] = ScreenVertex( x+jiggle+texturewidth, y+textureheight, 1, color, 1, 1 );
	v[1] = ScreenVertex( x+jiggle,   y+jiggle+textureheight, 1,		color, 0, 1 );
	v[3] = ScreenVertex( x+jiggle,   y+jiggle, 1,					color, 0, 0 );
	v[5] = ScreenVertex( x+jiggle+texturewidth, y+jiggle,   1,		color, 1, 0 );
	v[4] = ScreenVertex( x+jiggle+texturewidth, y+textureheight+jiggle, 1, color, 1, 1 );

	SetTexture(Image, 0);
	SetFVF(D3DFVF_SCREENVERTEX);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	
//	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
//	D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	//D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, v,  sizeof(ScreenVertex));

	ResetWireframeState();

		// Figure out the vertices for the main window
	/*ScreenVertex WindowVertices[4];

	WindowVertices[0].z   = 0;WindowVertices[1].z   = 0;WindowVertices[2].z   = 0;WindowVertices[3].z   = 0;
	WindowVertices[0].RHW = 1;	WindowVertices[1].RHW = 1; WindowVertices[2].RHW = 1; WindowVertices[3].RHW = 1;
	WindowVertices[0].color = D3DCOLOR_RGBA(255, 255, 255, alpha);WindowVertices[1].color = D3DCOLOR_RGBA(255, 255, 255, alpha);
	WindowVertices[2].color = D3DCOLOR_RGBA(255, 255, 255, alpha);WindowVertices[3].color = D3DCOLOR_RGBA(255, 255, 255, alpha);

	WindowVertices[0].x   = x;
	WindowVertices[0].y   = y;
	WindowVertices[1].x   = x+texturewidth;
	WindowVertices[1].y   = y;
	WindowVertices[2].x   = x;
	WindowVertices[2].y   = y+textureheight;
	WindowVertices[3].x   = x+texturewidth;
	WindowVertices[3].y   = y+textureheight;



	WindowVertices[0].tu  = 0;
	WindowVertices[0].tv  = 0;
	WindowVertices[1].tu  = 1;
	WindowVertices[1].tv  = 0;
	WindowVertices[2].tu  = 0;
	WindowVertices[2].tv  = 1;
	WindowVertices[3].tu  = 1;
	WindowVertices[3].tv  = 1;


	SetTexture(Image, 0);
	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	SetFVF(D3DFVF_SCREENVERTEX);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, WindowVertices,  sizeof(ScreenVertex));*/
}

void Screen3D::DrawSprite( int x, int y, TextureHandle Image, int alpha)
{
	Texture* t = this->TM.GetTexture( Image );
    this->DrawSprite( x, y, t, alpha );

}


void Screen3D::DrawRectangle( int x, int y, int w, int h, D3DCOLOR color )
{
	ScreenVertex v[6];
	v[0] = ScreenVertex( x,    y,   1, color, 0, 0 );
	v[2] = ScreenVertex( x+w,  y+h, 1, color, 1, 1 );
	v[1] = ScreenVertex( x,    y+h, 1, color, 0, 1 );
	v[3] = ScreenVertex( x,    y,   1, color, 0, 0 );
	v[5] = ScreenVertex( x+w,  y,   1, color, 1, 0 );
	v[4] = ScreenVertex( x+w,  y+h, 1, color, 1, 1 );

	SetTexture(NULL, 0);
	SetFVF(D3DFVF_SCREENVERTEX);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, v,  sizeof(ScreenVertex));
	ResetWireframeState();

}


void Screen3D:: DrawAllAlpha()
{
	AlphaRendering = false;

	
	SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	//SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);

	std::list<RenderBlock>::iterator i;

	while(DrawList.size())
	{
		i = DrawList.begin();
		RenderBlock r = (*i);
	
		D3DDevice->SetTransform( D3DTS_WORLDMATRIX(0), &r.m);
		DrawPrimitiveVB((*i).VB, (*i).IB);
		DrawList.erase(i);
	}

	SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

void Screen3D::DrawAlphaPrimitive( VertexBuffer* VB, Position& camera, Position& CentralLocation, IndexBuffer* IB  )
{
	

	RenderBlock block;
	block.VB = VB;
	block.IB = IB;

	block.m = (CentralLocation.GetMatrix()).Mat ;

	

	Vector3D DistVector;
	DistVector.x = camera.x - CentralLocation.x;
	DistVector.y = camera.y - CentralLocation.y;
	DistVector.z = camera.z - CentralLocation.z;

	block.DistFromCamera = DistVector.Mag();
		
	//sort it in now
	
	if(DrawList.size())
	{
		std::list<RenderBlock>::iterator i;
		i = DrawList.begin();
		bool Inserted=false;

		while(!Inserted )
		{
			if(block.DistFromCamera >= (*i).DistFromCamera )
			{
				DrawList.insert(i, block);
				Inserted = true;
			}
			else if(i == DrawList.end())
			{
				DrawList.push_back(block);
				Inserted = true;
			}
			else
				++i;
	
			
		}


	}
	else
	{
		DrawList.push_back(block);
	}

}
