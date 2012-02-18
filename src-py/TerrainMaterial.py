from OpenGL.GL import *
from glew import GL_COMBINE, glActiveTextureARB, GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB
import elementtree.ElementTree as ET
import string
import TextureManager
import Shader
import Settings
import TerrainLayer
import Mod

vs = """
void main()
{

    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
    gl_FogFragCoord = gl_Position.z;
}
"""
ps = """
#version 110

uniform sampler2D stage1, stage2, stage3;

void main()
{
    vec4 result, fog_color;
    vec4 texel1 = texture2D( stage1, gl_TexCoord[0].st );
    vec4 texel2 = texture2D( stage2, gl_TexCoord[1].st );
    vec4 texel3 = texture2D( stage3, gl_TexCoord[1].st );
    result.rgb = texel1.rgb * texel2.rgb;
    float fog = clamp( (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0 );
    result.rgb = gl_Fog.color.rgb*(1.0-fog) + result.rgb*fog;
    result.a = texel3.b;
    gl_FragColor = result;
}
"""


class TerrainMaterialBase(object):
    def __init__(self, modname, mapname): pass
    def start(self): pass
    def end(self): pass


class TerrainMaterialFixedFunction(TerrainMaterialBase):
    def __init__(self, modname, mapname ):
        self.layers = TerrainLayer.loadLayers(modname, mapname )
        self._colormapName = Mod.MapPath(modname,mapname) + 'colormap.png'

    def precacheGraphics(self):
        self.colormap
        for l in self.layers:
            l.precacheGraphics()

    @property
    def colormap(self): return TextureManager.GetTexture( self._colormapName )

    def start(self):
        glActiveTextureARB( GL_TEXTURE0_ARB )
        glEnable(GL_TEXTURE_2D)
        self.colormap.bind(0)

    def end(self):
        pass


class TerrainMaterialShaderV2(TerrainMaterialBase):
    """Represents a terrain material. Usually only one material per terrain"""
    def __init__(self, modname, mapname ):
        self._shaderProgram = None
        self.layers = TerrainLayer.loadLayers( modname, mapname )
        self._colormapName = Mod.MapPath(modname,mapname) + 'colormap.png'

    def precacheGraphics(self):
        global vs, ps
        self.shader
        self.colormap
        for l in self.layers:
            l.precacheGraphics()

    @property
    def shader(self):
        if self._shaderProgram is None:
            self._shaderProgram = Shader.MakeProgramFromSource( vs, ps )
            self._shaderProgram.uniform1i( "stage1", 0 )
            self._shaderProgram.uniform1i( "stage2", 1 )
            self._shaderProgram.uniform1i( "stage3", 2 )
        return self._shaderProgram

    @property
    def colormap(self): return TextureManager.GetTexture( self._colormapName )

    def start( self ):
        self.shader.use()

    def end( self ):
        self.shader.stop()

def TerrainMaterial( modname, mapname ):
    if Settings.UseShaders:
        return TerrainMaterialShaderV2( modname, mapname )
    else:
        return TerrainMaterialFixedFunction( modname, mapname )

