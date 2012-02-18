#import OpenGL.GL

#print dir( OpenGL.GL.ARB )
from OpenGL.GL import *
from cgkit.cgtypes import *
import GlTest
import Shader
import glew
from Numeric import array


class ShaderTest (GlTest.GlTestApp):
	def onInit(self):
		self.points = [[-0.5, -0.5, 0], [0.5, -0.5, 0], [0.0, 0.5, 0]]
		self.indices = [0,1,2]

	def onDraw( self ):
		glDisable(GL_TEXTURE_2D)
		glDisable(GL_LIGHTING)

		glVertexPointerf( self.points )
		glEnableClientState( GL_VERTEX_ARRAY )

		#glDrawElementsui( GL_TRIANGLES, self.indices )
		glDrawArrays( GL_TRIANGLES, 0, 3 )

		glEnable(GL_TEXTURE_2D )



GlTest.run( ShaderTest )

