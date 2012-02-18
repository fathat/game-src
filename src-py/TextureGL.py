from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

import ImageLoader

import Graphics
from Graphics import GraphicsCard

import glew


def LoadTexture( filename ):
    texture, image = LoadTextureAdvanced( filename )
    return texture

def LoadTextureAdvanced( filename ):
    image = ImageLoader.load(filename)
    ix = image.size[0]
    iy = image.size[1]
    image = image.convert("RGBA")
    img_str = image.tostring("raw", "RGBA", 0, 1)

    # Create Texture
    texture = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture)   # 2d texture (x and y size)

    glPixelStorei(GL_UNPACK_ALIGNMENT,1)
    #glTexImage2D(GL_TEXTURE_2D, 0, 4, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_str)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, ix, iy, GL_RGBA, GL_UNSIGNED_BYTE, img_str )
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
    GraphicsCard.setTextureAnisotropy( 8.0 )

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)
    return texture, image
