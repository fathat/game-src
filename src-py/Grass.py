import Terrain
import TextureManager
import AutomaticStruct
import random
from cgkit.cgtypes import vec3

from OpenGL.GL import *
from glew import *

vs = """
void main()
{

    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FogFragCoord = gl_Position.z;
}
"""
ps = """

uniform sampler2D stage1;

void main()
{
    vec4 result, fog_color;
    float fog;

    result = texture2D( stage1, gl_TexCoord[0].st );
    fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
    result = mix(gl_Fog.color, result, fog );

    gl_FragColor = result;
}
"""

class GrassVertex(object):
    def __init__(self, position, normal, tex2, color ):
        self.position = position
        self.normal   = normal
        self.tex2     = tex2
        self.color    = color

    def glVertex(self):
        pass

class GrassQuad(object):
    """A quad of grass"""
    def __init__(self, terrain, x, z, height, length, color ):
        #translate x and z coordinates into world coordinates
        mx = x*terrain.extent
        mz = z*terrain.extent

        #figure out a random direction for the grass blade to point in
        direction = vec3(random.random()-0.5, 0, random.random()-0.5 )
        try:
            direction.normalize()
        except ZeroDivisionError:
            direction = vec3(1,0,0)
        direction = direction * length

        #figure out vertex coordinates for the random vector
        center = vec3( mx, 0, mz)
        a = center - direction
        b = center + direction
        a.y = terrain.getWorldHeightValue(a.x, -a.z)
        b.y = terrain.getWorldHeightValue(b.x, -b.z)
        c = a + vec3( 0, height, 0)
        d = b + vec3( 0, height, 0)
        self.a = a
        self.b = b
        self.c = c
        self.d = d

        #store opengl value thingies
        self.vertexArray = [a.x, a.y, a.z,
                            b.x, b.y, b.z,
                            c.x, c.y, c.z,
                            d.x, d.y, d.z]

        self.texCoords=[0.0, 0.0,
                        1.0, 0.0,
                        1.0, 1.0,
                        0.0, 1.0]

        self.color = color


    def drawSimple(self):
        a,b,c,d = self.a,self.b,self.c,self.d

        glColor4f(*self.color)

        glTexCoord2f( 0.0, 1.0 )
        glVertex3f( a.x, a.y, a.z )

        glTexCoord2f( 1.0, 1.0 )
        glVertex3f( b.x, b.y, b.z )

        glTexCoord2f( 1.0, 0.0 )
        glVertex3f( d.x, d.y, d.z )

        glTexCoord2f( 0.0, 0.0 )
        glVertex3f( c.x, c.y, c.z )



    def drawArrays(self):
        #set the vertex array pointer
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointerf(  self.vertexArray )

        #set texcoord array for first texture stage
        glEnableClientState( GL_TEXTURE_COORD_ARRAY )
        glClientActiveTextureARB( GL_TEXTURE0_ARB )
        glTexCoordPointerf( self.texCoords )

        glDrawElementsui( GL_QUADS, 1 )

        glDisableClientState( GL_VERTEX_ARRAY )
        glDisableClientState( GL_TEXTURE_COORD_ARRAY )

    def draw(self):
        self.drawSimple()



class GrassPatch(object):
    def __init__(self, world, layer_index, terrain, density, height, length, grass_texture):
        self.grassTexture = TextureManager.GetTexture( grass_texture )
        self.grassQuads  = []
        self.vertexArray = []
        self.texArray    = []
        self.indexArray = range(density*4)

        base_grass_vis = 0.5
        for i in xrange(density):
            x = random.random()
            z = random.random()
            vis = world.layerVisibility( 0, x, z, )
            if vis < base_grass_vis:
                continue
            vis = (vis - base_grass_vis) / (1.0-base_grass_vis)
            shadow = world.lightIntensityAtPercentage(x,z)
            color = (shadow,shadow,shadow,1.0)
            new_quad = GrassQuad( terrain, x, z, height*vis, length*vis, color)
            self.vertexArray.extend( new_quad.vertexArray )
            self.texArray.extend( new_quad.texCoords )
            self.grassQuads.append( new_quad )

    def drawSimpleQuads(self):
        glBegin(GL_QUADS)
        for quad in self.grassQuads:
            #quad.drawSimple()
            quad.draw()
        glEnd()

    def drawArrays(self):
        #set the vertex array pointer
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointerf(  self.vertexArray )

        #set texcoord array for first texture stage
        glClientActiveTextureARB( GL_TEXTURE0_ARB )
        glTexCoordPointerf( self.texArray )
        glEnableClientState( GL_TEXTURE_COORD_ARRAY )

        glDrawElementsui( GL_QUADS, len(self.grassQuads) )

        glDisableClientState( GL_VERTEX_ARRAY )
        glDisableClientState( GL_TEXTURE_COORD_ARRAY )

    def draw(self):
        TextureManager.DisableStage(2)
        TextureManager.DisableStage(1)
        TextureManager.DisableStage(0)

        glDisable(GL_CULL_FACE)
        glDisable(GL_LIGHTING)
        glDisable(GL_BLEND)

        glEnable(GL_ALPHA_TEST)
        glAlphaFunc(GL_GREATER,0.1)

        self.grassTexture.bind(0)

        #self.drawArrays()
        self.drawSimpleQuads()

        glEnable(GL_ALPHA_TEST)
        glEnable(GL_BLEND)
        glEnable(GL_LIGHTING)
        glEnable(GL_CULL_FACE)
        glColor4f(1.0,1.0,1.0,1.0)

        TextureManager.DisableStage(0)
        #print "drawing...", len(self.indexArray)
        #glDisable(GL_CULL_FACE)
        #
        ##set the vertex array pointer
        #glEnableClientState(GL_VERTEX_ARRAY);
        #glVertexPointerf(  self.vertexArray )
        #
        ##set texcoord array for first texture stage
        #glEnableClientState( GL_TEXTURE_COORD_ARRAY )
        #glew.glClientActiveTextureARB( glew.GL_TEXTURE0_ARB )
        #glTexCoordPointerf( self.texArray )
        #
        #glDrawElementsui( GL_QUADS, self.indexArray )
        #
        #glDisableClientState( GL_VERTEX_ARRAY )
        #glDisableClientState( GL_TEXTURE_COORD_ARRAY )
