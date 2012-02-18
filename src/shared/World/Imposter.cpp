#include "Imposter.h"


Real CheckAngle( Real x, Real y )
{
	if(x>=0)
		return DEGREE(atan2f( x, y ));
	else
		return 360.0f+ DEGREE(atan2f(x,y));
}


ImposterType::ImposterType()
{
	AngleIncrement = 45;
	nTextures      = 0;
	TextureList    = NULL;
	hBaseMesh      = -1;
	nVertices      = 0;
}

ImposterType::~ImposterType()
{
	Cleanup();
}

bool ImposterType::GenerateTextures(Screen3D& Screen, MeshManager& MM,   Real angleIncrement, int texWidth, int texHeight, MeshHandle hSourceMesh )
{
	Cleanup();

	//make sure angleIncrement is valid
	if(fmod (360, angleIncrement) != 0)
		return false;

	//Set variables
	AngleIncrement = angleIncrement;
	hBaseMesh      = hSourceMesh;

	//Create the texture list
	TextureWidth  = texWidth;
	TextureHeight = texHeight;

	nTextures   = (int)(360.0f/AngleIncrement);
	if(nTextures < 0)
		return false;
	TextureList = new RenderTarget[nTextures];


	//get the mesh
	Mesh* pMesh = MM.Get( hBaseMesh );
	if(pMesh == NULL)
	{
		Cleanup();
		return false;
	}

	//Initialize the shared vertex buffer
	SharedVB.Init( Screen, D3DFVF_PARTICLEVERTEX, sizeof(ParticleVertex ), IMPOSTER_DRAW_BUFFER_CAPACITY, false, true, D3DPT_TRIANGLELIST, false );

	//Store the height of the mesh
	WorldHeight = (pMesh->Box.max.y - pMesh->Box.min.y);

	//setup stuff for rendering
	LPDIRECT3DSURFACE9 ScratchSurface, OriginalSurface, DepthSurface;
	LPDIRECT3DTEXTURE9 ScratchTexture;
	D3DXMATRIX projectionMatrix;
	Matrix TransformMat, RotationMat, ScaleMat, FinalMat, Identity;

	D3DXMatrixOrthoLH( &projectionMatrix, ((float)TextureWidth*(4.0f/3.0f)), TextureHeight, 1.0, 1000 );
	//D3DXMatrixOrthoLH( &projectionMatrix, pMesh->Box., TextureHeight, 1.0, 1000 );
	Screen.D3DDevice->SetTransform( D3DTS_PROJECTION, &projectionMatrix );

	Screen.SetViewTransform( Identity);

	Screen.SetClearColor(0, 0, 0, 0 );
	Screen.SetRenderState( D3DRS_ZENABLE, TRUE );
	Screen.SetRenderState( D3DRS_LIGHTING, TRUE );	

	// Set up the sun color
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r  = (Real)235;
	light.Diffuse.g  = (Real)235;
	light.Diffuse.b  = (Real)210;
	vecDir = D3DXVECTOR3(-0.0,
		-1.0f,
		0.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	light.Range       = 1000.0f;
	Screen.D3DDevice->SetLight( 0, &light );
	Screen.D3DDevice->LightEnable( 0, TRUE );
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	Screen.D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 50, 50, 50 ) );
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHAREF, 10);
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Screen.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	CRITICAL_SECTION criticalSection;
	InitializeCriticalSection( &criticalSection );


	//Generate imposters for each texture
	for(int i=0; i<nTextures; i++)
	{
		//init the texture
		TextureList[i].Init( &Screen, "imposter", TextureWidth, TextureHeight, D3DFMT_A8R8G8B8 );

		//Enter critical section
		Screen.LockScreenAccess();

		//Save the old render target
		Screen.D3DDevice->GetDepthStencilSurface( &DepthSurface );
		Screen.D3DDevice->GetRenderTarget(0, &OriginalSurface);

		//Set the new render target (our texture)
		ScratchTexture = TextureList[i].GetTextureSurface();
		ScratchTexture->GetSurfaceLevel(0, &ScratchSurface);
		Screen.D3DDevice->SetRenderTarget(0, ScratchSurface);


		//update the texture
		Screen.Clear(false );
		Screen.Start3D();


		//draw imposter
		Real scaleFactor = 1.0f/(pMesh->Box.max.y - pMesh->Box.min.y);
		float offset_y = (pMesh->Box.max.y -  pMesh->Box.min.y)/2  ;
		D3DXMatrixScaling( &ScaleMat.Mat, scaleFactor*TextureHeight, scaleFactor*TextureHeight, scaleFactor*TextureHeight );
		TransformMat.Translate( 0, -TextureHeight*0.5f, 500  );//(float)(-TextureHeight)*.5f
		RotationMat.Rotate( RADIAN((i*AngleIncrement)), 0, 0 );

		FinalMat = ScaleMat * (RotationMat*TransformMat); 

		Screen.SetTransform( FinalMat );
		pMesh->Draw( Screen );

		Screen.End3D();

		//Restore the old render target
		Screen.D3DDevice->SetRenderTarget( 0, OriginalSurface  );


		//Release surfaces we used
		ScratchSurface->Release();
		OriginalSurface->Release();

		Screen.UnlockScreenAccess();


	}



	//build the optimized texture
	int optTexWidth = sqrt( (float)nTextures )*texWidth;
	int optTexHeight= sqrt( (float)nTextures )*texHeight;
	nTexturesAcross = sqrt( (float)nTextures );
	nTexturesDown   = sqrt( (float)nTextures );

	OptimizedTexture.Init( &Screen, "optimizedImposter", optTexWidth, optTexHeight, D3DFMT_A8R8G8B8);


	Screen.LockScreenAccess();

	//copy all the textures into the optimized texture
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	//Save the old render target
	Screen.D3DDevice->GetDepthStencilSurface( &DepthSurface );
	Screen.D3DDevice->GetRenderTarget(0, &OriginalSurface);

	//Set the new render target (our texture)
	ScratchTexture = OptimizedTexture.GetTextureSurface();
	ScratchTexture->GetSurfaceLevel(0, &ScratchSurface);
	Screen.D3DDevice->SetRenderTarget(0, ScratchSurface);


	//Screen.Clear(false );
	Screen.D3DDevice->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0);
	Screen.Start3D();

	Screen.SetRenderState( D3DRS_ZENABLE, FALSE );

	int texindex = 0;
	RECT texRect;
	for(int y=0; y<nTexturesDown; y++)
	{
		for(int x=0; x<nTexturesAcross; x++)
		{
			texRect.left   = x*texWidth;
			texRect.top    = y*texHeight;
			texRect.right  = texRect.left + texWidth;
			texRect.bottom = texRect.top  + texHeight;

			Screen.DrawSpriteNoHandle( texRect, &TextureList[texindex], true, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );


			texindex ++;
		}
	}


	Screen.SetRenderState( D3DRS_ZENABLE, TRUE );
	Screen.End3D();

	//Restore the old render target
	Screen.D3DDevice->SetRenderTarget( 0, OriginalSurface  );


	//Release surfaces we used
	DepthSurface->Release ();
	ScratchSurface->Release();
	OriginalSurface->Release();

	Screen.UnlockScreenAccess();

	//delete the critical section
	DeleteCriticalSection( &criticalSection );

	return true;

}

void ImposterType::Cleanup()
{
	if(TextureList)
		delete [] TextureList;
	TextureList = NULL;
	nTextures   = 0;
	hBaseMesh   = -1;
}

Texture* ImposterType::GetTexture(int n)
{
	if(n >= nTextures)
		return NULL;
	if( n < 0 )
		return NULL;
	return &TextureList[n];
}


void ImposterType::Draw ( Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation )
{
	Matrix ScaleMat, RotationMat, TranslationMat, FinalMat;
	Vector3D vDir;
	vDir.x = impLocation->x-Viewer->Pos->x;
	vDir.y = impLocation->y-Viewer->Pos->y;
	vDir.z = impLocation->z-Viewer->Pos->z;

	Real angle = CheckAngle( vDir.x, vDir.z );

	Real corrected_angle =  angle - DEGREE(impLocation->Yaw);

	while(corrected_angle > 360.0f)
		corrected_angle -= 360.0f;
	while( corrected_angle < 0.0f )
		corrected_angle += 360.0f;

	int textureIndex = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures);
	Real blendFactor = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures)-textureIndex;

	ScaleMat.Scale( (4.0f/3.0f)*WorldHeight, WorldHeight, 1 );
	RotationMat.Rotate( RADIAN(angle), 0, 0 );
	TranslationMat.Translate( impLocation->x, impLocation->y, impLocation->z );
	FinalMat = ScaleMat* (RotationMat*TranslationMat);


	//Draw the something or other
	ParticleVertex v[4];
	v[0].position.x = -0.5f;
	v[0].position.y =  0.5f;
	v[0].position.z =  0;
	v[0].tu         = 0;
	v[0].tv         = 0;
	v[1].position.x =  0.5f;
	v[1].position.y =  0.5f;
	v[1].position.z =  0;
	v[1].tu         = 1;
	v[1].tv         = 0;
	v[2].position.x =  -0.5f;
	v[2].position.y =  -0.5f;
	v[2].position.z =  0;
	v[2].tu         = 0;
	v[2].tv         = 1;
	v[3].position.x =  0.5f;
	v[3].position.y = -0.5f;
	v[3].position.z =  0;
	v[3].tu         = 1;
	v[3].tv         = 1;

	int alpha        = 255.0f;
	v[0].color      = D3DCOLOR_ARGB(alpha, 255, 255, 255 );
	v[1].color      = D3DCOLOR_ARGB(alpha, 255, 255, 255 );
	v[2].color      = D3DCOLOR_ARGB(alpha, 255, 255, 255 );
	v[3].color      = D3DCOLOR_ARGB(alpha, 255, 255, 255 );

	Screen.SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	Screen.SetTexture( &TextureList[textureIndex], 0 );
	Screen.SetTransform ( FinalMat );
	//Screen.D3DDevice->SetVertexShader( D3DFVF_PARTICLEVERTEX );
	Screen.SetFVF(D3DFVF_PARTICLEVERTEX);
	Screen.D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, v, sizeof(ParticleVertex) );
	Screen.PolygonsRendered += 2;

	//Draw the next one in the sequence and such and such
	/*textureIndex--;
	if(textureIndex < 0)
	textureIndex = nTextures-1;

	v[0].color      = D3DCOLOR_ARGB(255-alpha, 255, 255, 255 );
	v[1].color      = D3DCOLOR_ARGB(255-alpha, 255, 255, 255 );
	v[2].color      = D3DCOLOR_ARGB(255-alpha, 255, 255, 255 );
	v[3].color      = D3DCOLOR_ARGB(255-alpha, 255, 255, 255 );

	Screen.SetTexture( &TextureList[textureIndex], 0 );
	Screen.D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, v, sizeof(ParticleVertex) );*/

}


void ImposterType::DrawOptimized( Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation )
{
	Matrix ScaleMat, RotationMat, TranslationMat, FinalMat;
	Vector3D vDir;
	vDir.x = impLocation->x-Viewer->Pos->x;
	vDir.y = impLocation->y-Viewer->Pos->y;
	vDir.z = impLocation->z-Viewer->Pos->z;

	Real angle = CheckAngle( vDir.x, vDir.z );

	Real corrected_angle =  angle - DEGREE(impLocation->Yaw);

	while(corrected_angle > 360.0f)
		corrected_angle -= 360.0f;
	while( corrected_angle < 0.0f )
		corrected_angle += 360.0f;

	int textureIndex = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures);
	Real blendFactor = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures)-textureIndex;

	ScaleMat.Scale( (4.0f/3.0f)*WorldHeight, WorldHeight, 1 );
	RotationMat.Rotate( RADIAN(angle), 0, 0 );
	TranslationMat.Translate( impLocation->x, impLocation->y, impLocation->z );
	FinalMat = ScaleMat* (RotationMat*TranslationMat);


	//Draw the something or other
	ParticleVertex v[4];
	v[0].position.x = -0.5f;
	v[0].position.y =  1;
	v[0].position.z =  0;

	v[1].position.x =  0.5f;
	v[1].position.y =  1;
	v[1].position.z =  0;
	v[2].position.x =  -0.5f;
	v[2].position.y =  0;
	v[2].position.z =  0;
	v[3].position.x =  0.5f;
	v[3].position.y = 0;
	v[3].position.z =  0;


	//convert the texture index into texture coordinates
	Real tx = (Real)(textureIndex % 4) / (Real)nTexturesAcross ;
	Real ty = (Real)(textureIndex / 4) / (Real)nTexturesDown ;

	v[0].tu         = tx;
	v[0].tv         = ty;
	v[1].tu         = tx + (1.0f/(Real)nTexturesAcross);
	v[1].tv         = ty;
	v[2].tu         = tx;
	v[2].tv         = ty + (1.0f/(Real)nTexturesDown);
	v[3].tu         = tx + (1.0f/(Real)nTexturesAcross);
	v[3].tv         = ty + (1.0f/(Real)nTexturesDown);


	int alpha        = blendFactor*255.0f;
	v[0].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[1].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[2].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[3].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );

	Screen.SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	Screen.SetTexture( &OptimizedTexture, 0 );
	Screen.SetTransform ( FinalMat );
	Screen.SetFVF( D3DFVF_PARTICLEVERTEX );
	Screen.D3DDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, v, sizeof(ParticleVertex) );
}



//ultra superfast drawing stuff
void ImposterType::WriteVertices(  Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation )
{

	//not enough space
	if(nVertices+6 >= IMPOSTER_DRAW_BUFFER_CAPACITY )
		return;

	//stuff write vertex buffer etc. etc. as such per previously agreed contract
	//in such stated terms ¬_¬
	Matrix ScaleMat, RotationMat, TranslationMat, FinalMat;
	Vector3D vDir;
	vDir.x = impLocation->x-Viewer->Pos->x;
	vDir.y = impLocation->y-Viewer->Pos->y;
	vDir.z = impLocation->z-Viewer->Pos->z;

	Real angle = CheckAngle( vDir.x, vDir.z );

	Real corrected_angle =  angle - DEGREE(impLocation->Yaw);

	while(corrected_angle > 360.0f)
		corrected_angle -= 360.0f;
	while( corrected_angle < 0.0f )
		corrected_angle += 360.0f;

	int textureIndex = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures);
	Real blendFactor = (((360.0f-corrected_angle)/360.0f) * (Real)nTextures)-textureIndex;

	ScaleMat.Scale( (4.0f/3.0f)*WorldHeight, WorldHeight, 1 );
	RotationMat.Rotate( RADIAN(angle), 0, 0 );
	TranslationMat.Translate( impLocation->x, impLocation->y, impLocation->z );
	FinalMat = ScaleMat* (RotationMat*TranslationMat);


	//set a pointer to the data we're editing
	ParticleVertex* v= &DrawBuffer[nVertices];

	//write some position stuff into the thing and such
	v[0].position.x = -0.5f;
	v[0].position.y =  0.5f+0.5f;
	v[0].position.z =  0;
	v[1].position.x =  0.5f;
	v[1].position.y =  0.5f+0.5f;
	v[1].position.z =  0;
	v[2].position.x =  -0.5f;
	v[2].position.y =  -0.5f+0.5f;
	v[2].position.z =  0;
	v[3].position.x =  -0.5f;
	v[3].position.y =  -0.5f+0.5f;
	v[3].position.z =  0;
	v[4].position.x =  0.5f;
	v[4].position.y =  0.5f+0.5f;
	v[4].position.z =  0;
	v[5].position.x =  0.5f;
	v[5].position.y =  -0.5f+0.5f;
	v[5].position.z =  0;


	//translate all the vertices
	D3DXVECTOR4 storageVector;
	for(int i=0; i<6; i++)
	{
		D3DXVec3Transform( &storageVector, &v[i].position, &FinalMat.Mat );
		v[i].position.x = storageVector.x;
		v[i].position.y = storageVector.y;
		v[i].position.z = storageVector.z;
	}

	//convert the texture index into texture coordinates
	Real tx = (Real)(textureIndex % 4) / (Real)nTexturesAcross ;
	Real ty = (Real)(textureIndex / 4) / (Real)nTexturesDown ;


	//set texture coordinates
	v[0].tu         = tx;
	v[0].tv         = ty;
	v[1].tu         = tx + (1.0f/(Real)nTexturesAcross);
	v[1].tv         = ty;
	v[2].tu         = tx;
	v[2].tv         = ty + (1.0f/(Real)nTexturesDown);
	v[3].tu         = tx;
	v[3].tv         = ty + (1.0f/(Real)nTexturesDown);
	v[4].tu         = tx + (1.0f/(Real)nTexturesAcross);
	v[4].tv         = ty;
	v[5].tu         = tx + (1.0f/(Real)nTexturesAcross);
	v[5].tv         = ty + (1.0f/(Real)nTexturesDown);


	//set vertex color
	v[0].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[1].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[2].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[3].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[4].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[5].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );
	v[6].color      = D3DCOLOR_ARGB(255, 255, 255, 255 );

	nVertices+=6;


}

void ImposterType::DrawStoredVertices( Screen3D& Screen, MeshManager& MM )
{

	//can't draw what's not there
	if(nVertices < 0 )
		return ;

	//Fill in the vertexbuffer
	HRESULT hr;
	ParticleVertex* v;
	hr = SharedVB.D3DBuffer()->Lock( 0, sizeof(ParticleVertex)*nVertices, (void**)&v, D3DLOCK_DISCARD );

	if(SUCCEEDED(hr))
	{
		memcpy(v, DrawBuffer, sizeof(ParticleVertex) * nVertices );
		SharedVB.Unlock();
	}

	//Draw stuff here
	Matrix Ident;
	Screen.SetTransform(Ident);

	Screen.SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	Screen.SetRenderState( D3DRS_ALPHAREF, 32 );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	Screen.SetTexture( &OptimizedTexture, 0 );
	Screen.SetFVF( D3DFVF_PARTICLEVERTEX );
	//Screen.D3DDevice->SetStreamSource( 0, SharedVB.VB, 0, sizeof(ParticleVertex));
	SharedVB.SetActive( 0 );
	Screen.D3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices/3);
	Screen.D3DDevice->SetStreamSource( 0, NULL,  0, NULL);

	Screen.PolygonsRendered += nVertices/3;
	Screen.DrawCalls++;

	//reset the vertex count
	nVertices = 0;
}