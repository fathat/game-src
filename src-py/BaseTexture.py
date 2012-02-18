import sys
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import glew
from TextureGL import LoadTextureAdvanced



class Texture( object ):
    def __init__(self, filename):
        self.id, self.image = LoadTextureAdvanced( filename )
        self.width  = self.image.size[0]
        self.height = self.image.size[1]

    def __str__(self ):
        return "%s,%s,%s" % ( str( self.id), str( self.width ), str( self.height ) )

    def bind( self, stage=-1 ):
        if stage != -1:
            glew.glActiveTextureARB( glew.GL_TEXTURE0_ARB + stage )
            glEnable( GL_TEXTURE_2D )
        glBindTexture( GL_TEXTURE_2D, self.id )
