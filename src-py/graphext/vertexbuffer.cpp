#include "vertexbuffer.h"

void VertexOffset( int size,  unsigned int type, int offset )
{
	glVertexPointer( size, type, 0,  (const GLvoid*)offset );
}

void NormalOffset(  int size, unsigned int type, int offset )
{
	glNormalPointer( type, 0,  (const GLvoid*)offset );
}

void ColorOffset( int size,  unsigned int type, int offset )
{
	glColorPointer( size, type, 0,  (const GLvoid*)offset );
}

void TextureOffset( int size,  unsigned int type, int offset )
{
	glTexCoordPointer( size, type, 0, (const GLvoid*)offset );
}

void IndexOffset( int size,  unsigned int type, int offset )
{
	glIndexPointer( type, 0, (const GLvoid*)offset );
}

void DrawElementsVB( unsigned int mode, unsigned int count, unsigned int type, int offset)
{
	glDrawElements( mode, count, type, (const GLvoid*)offset);
}

unsigned int GetGLError()
{
	return glGetError();
}
const char* GLErrorString( unsigned int error )
{
	return (const char*)gluErrorString( error );
}

