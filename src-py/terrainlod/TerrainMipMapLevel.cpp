#include <math.h>
#include "TerrainMipMapLevel.h"
#include "TerrainPatch.h"
#include "TerrainMap.h"

int TerrainMipMapLevel::Length() {
	return m_baseGrid->Size();
}
float TerrainMipMapLevel::WorldLength() {
	return (m_baseGrid->Size()-1)*Spacing();
}

float TerrainMipMapLevel::Spacing() {
	return m_spacing;
}

void TerrainMipMapLevel::Spacing(float value ) {
	m_spacing = value; 
	//UpdateVertexHeights(); 
	InitVertexHeights();
}


float TerrainMipMapLevel::Interpolation() {
	return m_interpolation ; 
}

void TerrainMipMapLevel::Interpolation( float value) {
	m_interpolation = value; 
	//UpdateVertexHeights() ;
	//InitVertexHeights();
}


TerrainMipMapLevel::TerrainMipMapLevel( HeightGrid* aGrid, float spacing)
{
	Init( NULL, aGrid, spacing );
}	    

TerrainMipMapLevel::TerrainMipMapLevel( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing)
{
	Init( aPatch, aGrid, spacing );
}	    


TerrainMipMapLevel::~TerrainMipMapLevel()
{
	Cleanup();
}

void TerrainMipMapLevel::Init( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing)
{
	m_parentPatch	 = aPatch;
	m_baseGrid       = aGrid->Duplicate();
	m_simplifiedGrid = m_baseGrid->MakeSimplificationTarget();
	m_spacing		 = spacing;
	m_interpolation  = 0.0f;

	m_vertices   = MakeVertices();
	m_vertexBuffer.Init(m_baseGrid->Size()*m_baseGrid->Size());

	m_targetVertexBuffer.Init(m_simplifiedGrid->Size()*m_simplifiedGrid->Size());

	m_indexBlock = new TerrainIndexBlock( aGrid->Size()  );

	InitVertexHeights();
}	    

void TerrainMipMapLevel::Cleanup()
{
	delete m_baseGrid;
	if( m_simplifiedGrid != NULL )
		delete m_simplifiedGrid;
	if( m_indexBlock != NULL )
		delete m_indexBlock;
	if( m_vertices != NULL)
		delete m_vertices;

	m_simplifiedGrid = NULL;
	m_indexBlock	 = NULL;
	m_vertices		 = NULL;
}


//float TerrainMipMapLevel::CalcDrawDistance()
//{
	//return 0;

//}

float TerrainMipMapLevel::GetCurrentHeight( int x, int y )
{
	float a = m_baseGrid->GetHeight( x, y );
	float b = m_simplifiedGrid->GetHeight(x, y);

	return a + (b-a)*m_interpolation;

}

float TerrainMipMapLevel::GetTargetHeight(int x, int y)
{
	return m_simplifiedGrid->GetHeight(x, y);
}

float TerrainMipMapLevel::GetCurrentInterpolatedHeight( float x, float y )
{
	return 0;    
}

TerrainLODVertex* TerrainMipMapLevel::MakeVertices()
{
	TerrainLODVertex* v = new TerrainLODVertex[m_baseGrid->Size()*m_baseGrid->Size()];
	for( int y=0; y<m_baseGrid->Size(); y++ )
	{
		for( int x=0; x<m_baseGrid->Size(); x++ )
		{
			v[x+y*m_baseGrid->Size()] = TerrainLODVertex(  
				(Vector3D ( x*m_spacing, m_baseGrid->GetHeight( x, y ), y*m_spacing ))  );
		}
	}
	return v;
}

void TerrainMipMapLevel::SetIndexBlockSimplification( int simplifyWest, int simplifyEast, int simplifyNorth, int simplifySouth )
{

	m_indexBlock->m_westSimplificationLevel  = (int)pow((float)2, MAX( simplifyWest, 0 ) );
	m_indexBlock->m_eastSimplificationLevel  = (int)pow((float)2, MAX( simplifyEast, 0 ) );
	m_indexBlock->m_northSimplificationLevel = (int)pow((float)2, MAX( simplifyNorth, 0 ) );
	m_indexBlock->m_southSimplificationLevel = (int)pow((float)2, MAX( simplifySouth, 0 ) );	

}

void TerrainMipMapLevel::UpdateIndices()
{
	m_indexBlock->UpdateIndices();
}

void TerrainMipMapLevel::InitVertexHeights()
{
	TerrainLODVertex* a, *b; 
	a = m_vertexBuffer.vertexArray;
	b = m_vertexBuffer.vertexArray;

	for( int y=0; y<m_baseGrid->Size(); y++ )
	{
		for( int x=0; x<m_baseGrid->Size(); x++ )
		{
			//Get map coordinates (for texturing)
			float mx=(float)x/(float)(m_baseGrid->Size()-1), my=(float)y/((float)m_baseGrid->Size()-1);

			if(m_parentPatch != NULL)
			{
				float scale = 1.0f/ m_parentPatch->ParentTerrain()->PatchesAcross();
				mx = (float)m_parentPatch->Column() / (float)m_parentPatch->ParentTerrain()->PatchesAcross() + mx*scale;
				my = (float)m_parentPatch->Row()	/ (float)m_parentPatch->ParentTerrain()->PatchesAcross() + my*scale;
			}


			//Setup the vertices
			float targetHeight = m_simplifiedGrid->GetHeight(x, y);
		
			if( x == 0 || x== m_baseGrid->Size() -1 ||  y == 0 || y == m_baseGrid->Size() -1 )
				targetHeight = m_baseGrid->GetHeight(x, y); 

			a[x+y*m_baseGrid->Size()].v = Vector3D( x*m_spacing, m_baseGrid->GetHeight(x, y), y*m_spacing );		
			b[x+y*m_baseGrid->Size()].v = Vector3D( x*m_spacing, targetHeight, y*m_spacing );	
			a[x+y*m_baseGrid->Size()].tex0 = Vector2D( ((float)x/(m_baseGrid->Size()-1))*4 , ((float)y/(m_baseGrid->Size()-1)) *4 );
			a[x+y*m_baseGrid->Size()].tex1 = Vector2D( (float)mx , (float)my );
			a[x+y*m_baseGrid->Size()].tex2 = Vector2D( (float)mx , (float)my );
			b[x+y*m_baseGrid->Size()].tex0 = Vector2D( ((float)x/(m_baseGrid->Size()-1))*4 , ((float)y/(m_baseGrid->Size()-1)) *4 );
			b[x+y*m_baseGrid->Size()].tex1 = Vector2D( (float)mx , (float)my );
			b[x+y*m_baseGrid->Size()].tex2 = Vector2D( (float)mx , (float)my );
			
		}
	}
}

void TerrainMipMapLevel::UpdateVertexHeights()
{
	
}

void TerrainMipMapLevel::Draw()
{

	//Get the various matrices
	/*D3DXMATRIX matWorld, matView, matProj;
	m_screenPtr->D3DDevice->GetTransform( D3DTS_WORLD, &matWorld  );
	m_screenPtr->D3DDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	m_screenPtr->D3DDevice->GetTransform( D3DTS_VIEW, &matView );
	
	//set the combined world/proj/view matrix (for transformations and such)
	D3DXMATRIX compoundMatrix;
	D3DXMatrixMultiply(&compoundMatrix, &matWorld, &matView);
	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &matProj);
	m_screenPtr->SetVertexShaderConstant(0, &compoundMatrix);

	//set blend factor
	D3DXVECTOR4 blendFactor(m_interpolation, 1.0f - m_interpolation, 0.0f, 0.0f);
	m_screenPtr->SetVertexShaderConstant(9, &blendFactor, 1);
	m_screenPtr->SetVertexShaderConstant(9, &blendFactor, 1);

	D3DXVECTOR4 fog(100, 1000, 1/7000.0f, 1.0f );
	m_screenPtr->SetVertexShaderConstant(8, &fog, 1);

	D3DXVECTOR4 lightcolor( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR4 materialcolor( 1.0f, 1.0f, 1.0f, 1.0f );
	m_screenPtr->SetVertexShaderConstant(10, &lightcolor, 1);
	m_screenPtr->SetVertexShaderConstant(11, &materialcolor, 1);

	
	//m_screenPtr->D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	//m_screenPtr->D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//m_screenPtr->D3DDevice->SetVertexShader( D3DFVF_TERRAINLODVERTEX );
	m_screenPtr->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_screenPtr->SetRenderState( D3DRS_FOGENABLE, TRUE );

	m_screenPtr->D3DDevice->SetIndices( m_indexBlock->DeviceBuffer()->GetD3DBufferInterface() );
	m_screenPtr->D3DDevice->SetStreamSource( 0, m_vertexBuffer.D3DBuffer(), 0,  sizeof( TerrainLODVertex ) );
	m_screenPtr->D3DDevice->SetStreamSource( 1, m_targetVertexBuffer.D3DBuffer(), 0, sizeof( TerrainLODVertex ) );
	m_screenPtr->D3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_vertexBuffer.VertexCount(), 0, m_indexBlock->IndexCount()/3 );
	m_screenPtr->D3DDevice->SetStreamSource( 0, NULL, 0, 0 );
	m_screenPtr->D3DDevice->SetStreamSource( 1, NULL, 0, 0 );

	//m_screenPtr->DrawPrimitiveVB( &this->m_vertexBuffer, m_indexBlock->DeviceBuffer(), D3DPT_TRIANGLELIST, D3DFVF_TERRAINLODVERTEX);
	//m_screenPtr->D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_screenPtr->SetRenderState( D3DRS_FOGENABLE, TRUE);
	m_screenPtr->SetRenderState( D3DRS_LIGHTING, TRUE );
*/
}