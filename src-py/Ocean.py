import Platform
import TextureManager
from OpenGL.GL import glBegin, glTexCoord2f, glVertex3f, glEnd, glEnable, glDisable, glDepthMask
from OpenGL.GL import glBlendFunc, glColor4
from OpenGL.GL import GL_CULL_FACE, GL_QUADS, GL_LIGHTING, GL_TRUE, GL_FALSE
from OpenGL.GL import GL_BLEND, GL_ALPHA_TEST, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA

class Ocean(object):
	"""Draws an ocean"""

	def __init__(self, sea_level, texture_name, extent):
		self.seaLevel = sea_level
		self.textureName = texture_name
		self.extent = extent
		self.flowOffset = 0

	def update(self, timestep):
		self.flowOffset += timestep*(1.0/10.0)

	def _drawQuad(self, tex_offset):
		extent = self.extent
		y = self.seaLevel
		max_tex = 50.0+tex_offset
		glBegin(GL_QUADS)
		glTexCoord2f( 0.0+tex_offset, 0.0+tex_offset )
		glVertex3f( 0.0, y, 0.0 )
		glTexCoord2f( max_tex, 0.0+tex_offset )
		glVertex3f( extent, y, 0.0 )
		glTexCoord2f( max_tex, max_tex )
		glVertex3f( extent, y, extent )
		glTexCoord2f( 0.0+tex_offset, max_tex )
		glVertex3f( 0.0, y, extent )
		glEnd()

	def draw(self):

		TextureManager.GetTexture( Platform.asOSPath(self.textureName)).bind()
		glDisable(GL_CULL_FACE)
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
		glDisable(GL_ALPHA_TEST)
		glDisable(GL_LIGHTING)
		glDepthMask( GL_FALSE )
		glColor4(1, 1, 1, 0.25)
		self._drawQuad(self.flowOffset)
		glColor4(1, 1, 1, 0.25)
		self._drawQuad(-self.flowOffset)
		glDepthMask( GL_TRUE )
		glEnable(GL_CULL_FACE)
