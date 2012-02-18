#include "TerrainMipMapLevel.h"
#include "TerrainPatch.h"
#include "TerrainMap.h"
#include "..\CVar\CVar.h"

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


TerrainMipMapLevel::TerrainMipMapLevel( HeightGrid* aGrid, float spacing, Screen3D* screen)
{
	Init( NULL, aGrid, spacing, screen );
}	    

TerrainMipMapLevel::TerrainMipMapLevel( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing, Screen3D* screen)
{
	Init( aPatch, aGrid, spacing, screen );
}	    


TerrainMipMapLevel::~TerrainMipMapLevel()
{
	Cleanup();
}

void TerrainMipMapLevel::Init( TerrainPatch* aPatch, HeightGrid* aGrid, float spacing, Screen3D* screen)
{
	m_parentPatch	 = aPatch;
	m_baseGrid       = aGrid->Duplicate();
	m_simplifiedGrid = m_baseGrid->MakeSimplificationTarget();
	m_spacing		 = spacing;
	m_interpolation  = 0.0f;

	//CVar.WriteLine( "Creating vertex buffer... %d x %d", m_baseGrid->Size(), m_baseGrid->Size() );

	m_screenPtr  = screen;
	m_vertices   = MakeVertices();
	if( !m_vertexBuffer.Init
		( *screen, 
		0, 
		sizeof(TerrainLODVertex), 
		m_baseGrid->Size()*m_baseGrid->Size(),
		false,
		true
		) )
	{
		CVar.WriteLine( "TerrainMipMapLevel: Failed to initialize vertex buffer" );
	}

	if( !m_targetVertexBuffer.Init
		( *screen, 
		0, 
		sizeof(TerrainLODVertex), 
		m_simplifiedGrid->Size()*m_simplifiedGrid->Size(),
		false,
		true
		) )
	{
		CVar.WriteLine( "TerrainMipMapLevel: Failed to initialize target vertex buffer" );
	}

	m_indexBlock = new TerrainIndexBlock( screen,  aGrid->Size()  );

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
		delete []m_vertices;

	m_simplifiedGrid = NULL;
	m_indexBlock	 = NULL;
	m_vertices		 = NULL;
}


//float TerrainMipMapLevel::CalcDrawDistance()
//{
	//return 0;

//}


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
				(D3DXVECTOR3&) (Vector3D ( x*m_spacing, m_baseGrid->GetHeight( x, y ), y*m_spacing ))  );
		}
	}
	return v;
}

void TerrainMipMapLevel::SetIndexBlockSimplification( int simplifyWest, int simplifyEast, int simplifyNorth, int simplifySouth )
{

	m_indexBlock->m_westSimplificationLevel  = (int)pow( 2.0, MAX( simplifyWest, 0 ) );
	m_indexBlock->m_eastSimplificationLevel  = (int)pow( 2.0, MAX( simplifyEast, 0 ) );
	m_indexBlock->m_northSimplificationLevel = (int)pow( 2.0, MAX( simplifyNorth, 0 ) );
	m_indexBlock->m_southSimplificationLevel = (int)pow( 2.0, MAX( simplifySouth, 0 ) );	

}

void TerrainMipMapLevel::UpdateIndices()
{
	m_indexBlock->UpdateIndices();
}

void TerrainMipMapLevel::InitVertexHeights()
{
	TerrainLODVertex* a, *b; 
	m_vertexBuffer.Lock( (BYTE**)&a);//, D3DLOCK_DISCARD );
	m_targetVertexBuffer.Lock( (BYTE**)&b);//, D3DLOCK_DISCARD );

	for( int y=0; y<m_baseGrid->Size(); y++ )
	{
		for( int x=0; x<m_baseGrid->Size(); x++ )
		{
			float mx=(float)x/(float)(m_baseGrid->Size()-1), my=(float)y/((float)m_baseGrid->Size()-1);

			if(m_parentPatch != NULL)
			{
				float scale = 1.0f/ m_parentPatch->ParentTerrain()->PatchesAcross();
				mx = (float)m_parentPatch->Column() / (float)m_parentPatch->ParentTerrain()->PatchesAcross() + mx*scale;
				my = (float)m_parentPatch->Row()	/ (float)m_parentPatch->ParentTerrain()->PatchesAcross() + my*scale;
			}
			float targetHeight = m_simplifiedGrid->GetHeight(x, y);

			
			if( x == 0 || x== m_baseGrid->Size() -1 ||  y == 0 || y == m_baseGrid->Size() -1 )
				targetHeight = m_baseGrid->GetHeight(x, y); 
			a[x+y*m_baseGrid->Size()].v = D3DXVECTOR3( x*m_spacing, m_baseGrid->GetHeight(x, y), y*m_spacing );		
			b[x+y*m_baseGrid->Size()].v = D3DXVECTOR3( x*m_spacing, targetHeight, y*m_spacing );	
			a[x+y*m_baseGrid->Size()].tex0 = D3DXVECTOR2( ((float)x/(m_baseGrid->Size()-1))*4 , ((float)y/(m_baseGrid->Size()-1)) *4 );
			a[x+y*m_baseGrid->Size()].tex1 = D3DXVECTOR2( (float)mx , (float)my );
			a[x+y*m_baseGrid->Size()].tex2 = D3DXVECTOR2( (float)mx , (float)my );
			b[x+y*m_baseGrid->Size()].tex0 = D3DXVECTOR2( ((float)x/(m_baseGrid->Size()-1))*4 , ((float)y/(m_baseGrid->Size()-1)) *4 );
			b[x+y*m_baseGrid->Size()].tex1 = D3DXVECTOR2( (float)mx , (float)my );
			b[x+y*m_baseGrid->Size()].tex2 = D3DXVECTOR2( (float)mx , (float)my );
			
		}
	}

	
	m_targetVertexBuffer.Unlock();
	m_vertexBuffer.Unlock();
}
void TerrainMipMapLevel::UpdateVertexHeights()
{
	float interpolatedHeight;
	for( int y=1; y<m_baseGrid->Size()-1; y++ )
	{
		for( int x=1; x<m_baseGrid->Size()-1; x++ )
		{

			float baseGridHeight = m_baseGrid->GetHeight( x, y );
			float simplifiedGridHeight = m_simplifiedGrid->GetHeight( x, y );
			interpolatedHeight =  baseGridHeight + (simplifiedGridHeight-baseGridHeight)*m_interpolation;
			m_vertices[x+y*m_baseGrid->Size()].v.x = x*m_spacing;
			m_vertices[x+y*m_baseGrid->Size()].v.y = interpolatedHeight;
			m_vertices[x+y*m_baseGrid->Size()].v.z = y*m_spacing;
			m_vertices[x+y*m_baseGrid->Size()].tex0 = D3DXVECTOR2( (float)x, (float)y );
			m_vertices[x+y*m_baseGrid->Size()].tex1 = D3DXVECTOR2( (float)x, (float)y);
			m_vertices[x+y*m_baseGrid->Size()].tex2 = D3DXVECTOR2( (float)x, (float)y);
			
		}
	}

	TerrainLODVertex* vs; 
	m_vertexBuffer.Lock( (BYTE**)&vs, D3DLOCK_DISCARD );
	memcpy(vs, m_vertices, sizeof(TerrainLODVertex) * m_baseGrid->Size() * m_baseGrid->Size() );
	m_vertexBuffer.Unlock();

}

void TerrainMipMapLevel::Draw()
{

	//Get the various matrices
	D3DXMATRIX matWorld, matView, matProj;
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

	m_screenPtr->D3DDevice->SetIndices( m_indexBlock->DeviceBuffer()->GetD3DBufferInterface() );
	m_screenPtr->D3DDevice->SetStreamSource( 0, m_vertexBuffer.D3DBuffer(), 0,  sizeof( TerrainLODVertex ) );
	m_screenPtr->D3DDevice->SetStreamSource( 1, m_targetVertexBuffer.D3DBuffer(), 0, sizeof( TerrainLODVertex ) );
	m_screenPtr->D3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_vertexBuffer.VertexCount(), 0, m_indexBlock->IndexCount()/3 );
	m_screenPtr->D3DDevice->SetStreamSource( 0, NULL, 0, 0 );
	m_screenPtr->D3DDevice->SetStreamSource( 1, NULL, 0, 0 );

	m_screenPtr->PolygonsRendered +=m_indexBlock->IndexCount()/3;
	m_screenPtr->DrawCalls++;


}