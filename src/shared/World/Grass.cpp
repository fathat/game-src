#include "Grass.h"
#include "QuadTree.h"
#include "Terrain.h"

#define GRASS_HEIGHT 4.25f
#define GRASS_HEIGHT_VARIANCE 3.0f
#define GRASS_DRAW_DISTANCE 1000.0f

float frand()
{
	float r = (float)(rand()% 1000) / 1000.0f;
	return r;
}


GrassPatch::GrassPatch()
{
	m_opacityMap = NULL;
	m_screen	 = NULL;
}

GrassPatch::~GrassPatch()
{
	if( m_opacityMap)
		delete m_opacityMap;
}

bool GrassPatch::Init(Screen3D* screen, QuadNode* aNode, int density )
{
	m_screen = screen;
	m_opacityMap = aNode->Patch->Heights()->Duplicate();

	//load the grass texture
	m_grassTexture = m_screen->GetTextureHandle( "../base/art/land/grass_side_bw.png");

	float ax, ay, az, bx, by, bz;
	float mx_a, mz_a;
	float mx_b, mz_b;
	Vector3D normal;
	float size = m_opacityMap->Size();

	
	ParticleVertex* v;
	Index32* indices;
	
	v = new ParticleVertex[ density*4 ];
	indices = new Index32[ density * 6 ];

	int brightness_a;
	int brightness_b;
	int brightness_at;
	int brightness_bt;
	int n = 0;
	for ( int i=0; i<density; i++ )
	{	
		
		Vector3D arrow( frand()-0.5f, 0.000001f, frand()-0.5f );
		arrow.SetLength( .02f );

		ax = frand() ;
		az = frand() ;
		bx = ax + arrow.x;
		bz = az + arrow.z;

		//make sure the piece of grass doesn't extend outside of the node
		if( bx > 1.0f )
			bx = 1.0f;
		if( bz > 1.0f )
			bz = 1.0f;
		if( bx < 0.0f)
			bx = 0.0f;
		if ( bz < 0.0f)
			bz = 0.0f;

		mx_a   =  ax*(m_opacityMap->Size()-1);
		mz_a   =  az*(m_opacityMap->Size()-1);
		mx_b   =  bx*(m_opacityMap->Size()-1);
		mz_b   =  bz*(m_opacityMap->Size()-1);
		ay = m_opacityMap->GetInterpolatedHeight(mx_a , mz_a );
		by = m_opacityMap->GetInterpolatedHeight(mx_b , mz_b );
		
		float wx = aNode->Box.min.x + ax* ( aNode->Box.max.x - aNode->Box.min.x );
		float wz = aNode->Box.min.z + az* ( aNode->Box.max.z - aNode->Box.min.z );
		float wxb = aNode->Box.min.x + bx* ( aNode->Box.max.x - aNode->Box.min.x );
		float wzb = aNode->Box.min.z + bz* ( aNode->Box.max.z - aNode->Box.min.z );

		float slope;
		aNode->m_terrain->GetHeightValue( Position(Vector3D(wx, 0, wz)), &slope );

		float vis  = aNode->m_terrain->GetLayerVisibility( 0, wx, wz );
		if ( vis < 0.65f )
			continue ; 

		
		float greenVariance = fabs(frand())*0.25 + 0.5;
		float redVariance = greenVariance*fabs(frand()) * 0.25f;
		float blueVariance = 0.0f;//fabs(frand());

		brightness_a  = (int)(pow((float)aNode->m_terrain->GetLightValue( wx,  wz), (float)1.5) * 255.0f );
		brightness_at = (int)(brightness_a * 0.333f);
		brightness_b  = (int)(pow((float)aNode->m_terrain->GetLightValue( wxb, wzb), 1.5f) * 255.0f );
		brightness_bt = (int)(brightness_b * 0.333f );
		

		
		//arrow.SetLength( 15 );
		float heightVariance = frand() * GRASS_HEIGHT_VARIANCE  ;
		

		v[n*4+0].position = D3DXVECTOR3( wx, 
										 ay,  
										 wz);
		v[n*4+0].color = D3DCOLOR_ARGB( 255, (int)(brightness_at*redVariance), (int)(brightness_at*greenVariance), (int)(brightness_at*blueVariance));
		v[n*4+0].tu = 0;
		v[n*4+0].tv = 1;

		v[n*4+1].position = D3DXVECTOR3(wxb, 
										by,  
										wzb);
		v[n*4+1].color = D3DCOLOR_ARGB( 255, (int)(brightness_bt*redVariance), (int)(brightness_bt*greenVariance), (int)(brightness_bt*blueVariance) );
		v[n*4+1].tu = 1;
		v[n*4+1].tv = 1;

		v[n*4+2].position = v[n*4+0].position + D3DXVECTOR3( 0, ((GRASS_HEIGHT+heightVariance)*vis),  0);
		v[n*4+2].color = D3DCOLOR_ARGB( 255, (int)(brightness_a*redVariance), (int)(brightness_a*greenVariance), (int)(brightness_a*blueVariance));
		v[n*4+2].tu = 0;
		v[n*4+2].tv = .05;

		v[n*4+3].position = v[n*4+1].position + D3DXVECTOR3(  0,  ((GRASS_HEIGHT+heightVariance)*vis), 0 );
		v[n*4+3].color = D3DCOLOR_ARGB( 255, (int)(brightness_b*redVariance), (int)(brightness_b*greenVariance), (int)(brightness_b*blueVariance));
		v[n*4+3].tu = 1;
		v[n*4+3].tv = .05;

		indices[n*6+0] = n*4+0;
		indices[n*6+1] = n*4+1;
		indices[n*6+2] = n*4+2;
		indices[n*6+3] = n*4+1;
		indices[n*6+4] = n*4+3;
		indices[n*6+5] = n*4+2;

		n++;

		

	}

	nPolygons = n;
	if(n == 0)
		return false;

	//Copy data into the vertex buffer
	ParticleVertex* deviceVertices;
	Index32* deviceIndices;
	m_vertexBuffer.Init( *screen, D3DFVF_PARTICLEVERTEX,  sizeof(ParticleVertex), n*4 );
	m_indexBuffer.Init( *screen, n*6, IB32Bit );

	m_vertexBuffer.Lock( (BYTE**)&deviceVertices );
	m_indexBuffer.Lock( &deviceIndices);

	memcpy ( deviceVertices, v, sizeof(ParticleVertex)*n*4 );
	memcpy ( deviceIndices, indices, sizeof(Index32)*n*6 );

	m_indexBuffer.Unlock();
	m_vertexBuffer.Unlock();

	delete []v;
	delete []indices;
	return true;
}


void GrassPatch::Draw()
{
	if(nPolygons == 0) return ;
	m_screen->SetTransform( Matrix::MakeIdentity() );
	m_screen->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_screen->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_screen->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	//Get the various matrices
	D3DXMATRIX matWorld, matView, matProj;
	m_screen->D3DDevice->GetTransform( D3DTS_WORLD, &matWorld  );
	m_screen->D3DDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	m_screen->D3DDevice->GetTransform( D3DTS_VIEW, &matView );

	//set the combined world/proj/view matrix (for transformations and such)
	D3DXMATRIX compoundMatrix;
	D3DXMatrixMultiply(&compoundMatrix, &matWorld, &matView);
	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &matProj);
	m_screen->SetVertexShaderConstant(0, &compoundMatrix);

	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_screen->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_screen->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_screen->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_screen->D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	m_screen->D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	m_screen->D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   FALSE);
	m_screen->D3DDevice->SetRenderState(D3DRS_FOGENABLE,   FALSE);

	//m_screen->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0 ,0 ));
	m_screen->SetRenderState( D3DRS_LIGHTING, FALSE);
	m_screen->SetTexture(m_grassTexture, 0);
		
	//m_screen->SetVertexShaderConstant(8, &D3DXVECTOR4 (100, GRASS_DRAW_DISTANCE, 1/m_screen->ViewDistance(), 1.0f ), 1);
	m_screen->SetVertexShaderConstant( 9, &D3DXVECTOR4( 0.0f, 1.0f/(500.0f), 1.0f, 0.0f ), 1 );

	m_screen->D3DDevice->SetIndices( m_indexBuffer.GetD3DBufferInterface() );
	m_vertexBuffer.SetActive(0);
	m_screen->D3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_vertexBuffer.VertexCount(), 0, m_indexBuffer.GetIndexCount()/3 );
	m_screen->PolygonsRendered += m_indexBuffer.GetIndexCount()/3;
	m_screen->DrawCalls++;
	m_screen->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_screen->SetRenderState( D3DRS_LIGHTING, TRUE);
}
