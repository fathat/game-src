
from OpenGL.GL import *
from cgkit.cgtypes import *
import GlTest

from glew import glBindBufferARB, glGenBuffersARB, glDeleteBuffersARB, glBufferDataARB, GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB, glClientActiveTextureARB, GL_TEXTURE0_ARB, glVertexPointerEXT

import array
import graphext

import VertexBuffer

class SimpleVB(object):
	def __init__(self, id, points):
		self.id = id
		self.datastring = array.array( 'f', points).tostring()
		glEnableClientState( GL_VERTEX_ARRAY )
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, self.id )
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, len( self.datastring ), self.datastring, GL_STATIC_DRAW_ARB );

	def bind(self):
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, self.id )
		graphext.VertexOffset( 3, GL_FLOAT, 0 )

class ComplexVBOTest (GlTest.GlTestApp):
	def onInit(self):
		self.points = [vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0.0, 0.5, 0)]
		self.normals = [vec3(0, 0, 1), vec3( 0, 0, 1), vec3( 0, 0, 1 ) ]
		self.colors  = [vec4(0,0,1,1), vec4(0,1,0,1), vec4(1,0,0,1) ]
		self.vbo = VertexBuffer.MakeStandardVertexBuffer( self.points, colors=self.colors )
		self.indexbuffer = VertexBuffer.IndexBufferObject( [0, 1, 2] )


	def onDraw( self ):
		glDisable(GL_TEXTURE_2D)
		glDisable(GL_LIGHTING)
		glEnableClientState( GL_VERTEX_ARRAY )
		self.vbo.bind()
		self.indexbuffer.draw()
		self.vbo.unbind()
		glEnable(GL_TEXTURE_2D )


class VBOTest (GlTest.GlTestApp):
	def onInit(self):
		self.points = [-0.5, -0.5, 0, 0.5, -0.5, 0, 0.0, 0.5, 0]
		datablock = VertexBuffer.DataBlock( 'vertex', self.points )
		self.vb = VertexBuffer.VertexBufferObject( [datablock] )


	def onDraw( self ):
		glDisable(GL_TEXTURE_2D)
		glDisable(GL_LIGHTING)
		glEnableClientState( GL_VERTEX_ARRAY )
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, 1 )
		graphext.VertexOffset( 3, GL_FLOAT, 0 )
		glDrawArrays(GL_TRIANGLES, 0, 3)
		glEnable(GL_TEXTURE_2D )



GlTest.run( ComplexVBOTest )

