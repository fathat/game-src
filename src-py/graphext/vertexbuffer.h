#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

extern "C" {
void VertexOffset( int size, unsigned int type, int offset );
void NormalOffset( int size,  unsigned int type, int offset );
void ColorOffset( int size,  unsigned int type, int offset );
void TextureOffset( int size,  unsigned int type, int offset );
void IndexOffset( int size,  unsigned int type, int offset );

void DrawElementsVB( unsigned int mode, unsigned int count, unsigned int type, int offset);

unsigned int GetGLError();
const char* GLErrorString( unsigned int error );
}

class DataBlock
{
};

class VertexBufferObject
{
	int id;
};

#endif

