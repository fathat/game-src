import sys
import os.path
import OpenGL.GL as GL
from DDSTexture import DDSTexture
from BaseTexture import Texture
import Platform
import glew
from DisplayList import DisplayList, IllegalDisplayListCommandException

textures = {}

#Keep statistics on the number of each type of extension we load
extensions = {}

def TexturesCurrentlyLoaded():
    return textures.values()

def AllTextureNames():
    return textures.keys()

def MakeTexture( filename ):
    global extensions
    DisplayList.checkForInvalidCall("Loading texture %s in a display list " % (filename))
    ext = os.path.splitext( filename )[1]
    if ext in extensions:
        extensions[ext] += 1
    else:
        extensions[ext] = 1
    if filename.endswith( '.dds' ):
        return DDSTexture( filename )
    else:
        return Texture( filename )

def GetTexture( filename ):
    global textures
    rval = None
    try:
        rval = textures[filename]
    except:
        textures[filename] = MakeTexture( filename )
        rval = textures[filename]
    return rval

def Precache( filename ):
    GetTexture(filename)

def BindTexture( filename, stage=0 ):
    tex = GetTexture(filename)
    if tex:
        tex.bind(stage)
    else:
        DisableStage(stage)

def DisableStage( stage ):
    glew.glActiveTextureARB( glew.GL_TEXTURE0_ARB + stage  )
    GL.glBindTexture(GL.GL_TEXTURE_2D, 0 )
    GL.glDisable( GL.GL_TEXTURE_2D )
    glew.glActiveTextureARB( glew.GL_TEXTURE0_ARB  )

#tex, img = LoadTextureAdvanced( '../base/art/land/grassnew.bmp', encoding="RGB" )
#print img.convert("RGBA")
