#include "VertexShader.h"
#include "Screen.h"

VertexShader::VertexShader()
{
	m_vertexShader		= NULL;
	m_vertexDeclaration = NULL;
	m_screen			= NULL;
}

VertexShader::~VertexShader()
{
	Cleanup();
}

bool VertexShader::Init( Screen3D* Screen, const char* shaderfilename,const D3DVERTEXELEMENT9* declaration )
{

	m_screen = Screen;

	//construct the declaration (if vertex declaration was provided)
	if( declaration != NULL )
		m_screen->D3DDevice->CreateVertexDeclaration( declaration, &m_vertexDeclaration);

	//Run the shader through the compiler
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pErrors;
	DWORD shaderHandle;


	HRESULT HR = D3DXAssembleShaderFromFileA( shaderfilename,
											NULL, 
											NULL, 
											NULL,
											&pCode, 
											&pErrors );
	
	if ( FAILED(HR) )
	{
		OutputDebugString( "Failed to assemble the vertex shader, errors:\n" );
		OutputDebugString( (char*)pErrors->GetBufferPointer() );
		OutputDebugString( "\n" );

		return false;
	}
	else
	{
		//Create the shader!
		m_screen->D3DDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &m_vertexShader )  ;
		pCode->Release();
	}

	return true;

}

void VertexShader::Cleanup()
{
	if(m_vertexShader)
		m_vertexShader->Release();
	if( m_vertexDeclaration)
		m_vertexDeclaration->Release();
}

void VertexShader::Activate()
{
	m_screen->D3DDevice->SetVertexShader( m_vertexShader);

	if( m_vertexDeclaration != NULL )
		m_screen->D3DDevice->SetVertexDeclaration( m_vertexDeclaration );
}