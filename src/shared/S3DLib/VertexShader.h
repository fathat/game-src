#pragma once 
#include <d3d9.h>
#include <d3dx9.h>

// D E F I N E S //////////////////////////////////////////////////

//forward declarations
class Screen3D;

// C L A S S E S //////////////////////////////////////////////////

//A vertex shader! Le gasp!
class VertexShader
{
	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DVertexDeclaration9* m_vertexDeclaration;

	Screen3D* m_screen;

public:
	VertexShader();
	~VertexShader();

	bool Init( Screen3D* Screen, const char* shaderfilename, const D3DVERTEXELEMENT9* declaration );
	void Cleanup();
    
	IDirect3DVertexShader9* D3DShader() { return m_vertexShader; }
	IDirect3DVertexDeclaration9* VertexDeclaration() { return m_vertexDeclaration; }
	
	void Activate();
    
};