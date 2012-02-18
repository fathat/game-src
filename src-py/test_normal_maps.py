from OpenGL.GL import *
from cgkit.cgtypes import *
import GlTest
import Shader
import glew
#from glew import *
from Numeric import array

import TextureManager

vs = """

void main()
{
	
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
} 
"""
ps = """

uniform sampler2D stage1, stage2, stage3;

void main()
{
	vec4 texel1, texel2, texel3;
	vec4 result;
	
	texel1 = texture2D( stage1, gl_TexCoord[0].st );
	texel2 = texture2D( stage2, gl_TexCoord[1].st );
	texel3 = texture2D( stage3, gl_TexCoord[1].st );
	result.rgb = texel1.rgb * texel2.rgb;
	//result.a = 1.0;
	result.a   = texel3.b;
	
	gl_FragColor = result;
}
"""

class ShaderTest (GlTest.GlTestApp):
# 	def __init__(self):
# 		GlTest.GlTestApp.__init__( self )
	
	def onInit(self):
		self.program = Shader.MakeProgramFromSource( vs, ps )
		self.program.printInfoLog()
		
		#setup lighting
		n = vec3(0, 0, 1 )
		n = n.normalize()
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, [0.8, 0.8, 0.8, 1.0] )
		glLightfv( GL_LIGHT0, GL_AMBIENT, [0.0, 0.0, 0.0, 1.0 ] )
		glLightfv( GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0 ] )
		glLightfv( GL_LIGHT0, GL_POSITION, [n.x, n.y, n.z, 1.0] )
		glEnable(GL_LIGHT0)
		
		#glew.glUseProgramObjectARB( 0 )
	
	def onDraw( self ):
		
		glEnable(GL_LIGHTING)
		TextureManager.GetTexture( "../base/art/land/sand.bmp" ).bind(0)
		TextureManager.GetTexture("../base/maps/pymap/shadows.png").bind(1)
		TextureManager.GetTexture("../base/maps/pymap/layer_dirt slope.png").bind(2)
		
		self.program.uniform1i( "stage1", 0 )
		self.program.uniform1i( "stage2", 1 )
		self.program.uniform1i( "stage3", 2 )
		self.program.use()
		
		
		glBegin( GL_TRIANGLES )
		#glTexCoord2f( 0.0, 0.0 )
		glColor3f( 1.0, 1.0, 1.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE0_ARB, 0.0, 0.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE1_ARB, 0.0, 0.0 )
		glNormal3f( 0, 0, 1.0 )
		glVertex3f( -0.5, -0.5, 0 )
		#glTexCoord2f( 1.0, 0.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE0_ARB, 1.0, 0.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE1_ARB, 1.0, 0.0 )
		glNormal3f( 0, 0, 1.0 )
		glVertex3f(  0.5, -0.5, 0 )
		#glTexCoord2f( 0.5, 1.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE0_ARB, 0.5, 1.0 )
		glew.glMultiTexCoord2f(glew.GL_TEXTURE1_ARB, 0.5, 1.0 )
		glNormal3f( 0, 0, 1.0 )
		glVertex3f(  0.0,  0.5, 0 )
		glEnd()
		
		self.program.stop()
		glDisable(GL_TEXTURE_2D)
		glDisable(GL_LIGHTING)
		TextureManager.DisableStage( 2 )
		TextureManager.DisableStage( 1 )
		

#print dir(glew )
GlTest.run( ShaderTest )
#print vs.size

#print glew.GL_TEXTURE0_ARB
#print glew.GL_TEXTURE5_ARB
