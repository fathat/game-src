from OpenGL.GL import *
from cgkit.cgtypes import *

class Sprite(object):
	
	def __init__ (self, texture, size ):
		self.texture = texture
		self.size = size 
	
	def drawAsParticle( sefl, position, viewMatrix ):
		rightVec = vec3( -viewMatrix[0,0], -viewMatrix[0,1], -viewMatrix[0,2] )
		upVec    = vec3( -viewMatrix[1,0], -viewMatrix[1,1], -viewMatrix[1,2] )
		vertices = self._constructVertiecs( position, upVec, rightVec )
		self._draw( vertices, viewMatrix ) 

	def drawAsBillboard( self, position, viewMatrix ):
		rightVec = vec3( -viewMatrix[0,0], -viewMatrix[0,1], -viewMatrix[0,2] )
		upVec    = vec3( 0, 1, 0) 
		vertices = self._constructVertices( position, upVec, rightVec )
		self._draw( vertices, viewMatrix )
	
	def _constructVertices ( self, position, upVec, rightVec ):
		u = upVec * 0.5 * self.size
		r = rightVec * 0.5 * self.size
		return [ position+u-r, position+u+r, position-u+r, position-u-r ]
	
	def _draw( self, vertices, viewMatrix ):
		glPushMatrix()
		glBindTexture( GL_TEXTURE_2D, self.texture )
		glLoadMatrixf( viewMatrix.toList() ) 
		self._drawQuad( vertices )
		glPopMatrix()
	
	def _drawQuad( self, vertices ):
		v1 = vertices[0]
		v2 = vertices[1]
		v3 = vertices[2]
		v4 = vertices[3]
		glBegin( GL_QUADS )
		glTexCoord2f( 0, 0 )
		glVertex3f( v1.x, v1.y, v1.z )
		glTexCoord2f( 1, 0 )
		glVertex3f( v2.x, v2.y, v2.z )
		glTexCoord2f( 1, 1 )
		glVertex3f( v3.x, v3.y, v3.z )
		glTexCoord2f( 0, 1 )
		glVertex3f( v4.x, v4.y, v4.z )
		glEnd()

