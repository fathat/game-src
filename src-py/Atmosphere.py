import TextureManager
import Platform
from Graphics import GraphicsCard
from OpenGL.GL import *
from OpenGL.GLU import gluPerspective


class SkyBox(object):
    def __init__(self, north, south, east, west, up, down):
        self.north = Platform.asOSPath(north)
        self.south = Platform.asOSPath(south)
        self.east  = Platform.asOSPath(east)
        self.west  = Platform.asOSPath(west)
        self.up    = Platform.asOSPath(up)
        self.down  = Platform.asOSPath(down)

    def drawQuad( self, top_left, top_right, bottom_left, bottom_right ):
        glBegin( GL_QUADS )
        glTexCoord2f( 0, 0 )
        glVertex3f( *top_left )
        glTexCoord2f( 1, 0 )
        glVertex3f( *top_right )
        glTexCoord2f( 1, 1 )
        glVertex3f( *bottom_right )
        glTexCoord2f( 0, 1 )
        glVertex3f( *bottom_left )
        glEnd()

    def draw(self, camera):
        glDisable(GL_DEPTH_TEST)
        glDisable(GL_LIGHTING)
        glDisable(GL_CULL_FACE)
        distance = 5

        glPushMatrix()
        glLoadIdentity()
        glScalef(1,1,-1)
        glMultMatrixf( camera.getRotationMatrix().inverse().toList() )
        glColor4(1,1,1,1)

        #construct points of cube..
        nwt = ( -distance,  distance, -distance )
        nwb = ( -distance, -distance, -distance )
        net = (  distance,  distance, -distance )
        neb = (  distance, -distance, -distance )
        swt = ( -distance,  distance,  distance )
        swb = ( -distance, -distance,  distance )
        set = (  distance,  distance,  distance )
        seb = (  distance, -distance,  distance )

        #draw the north face
        TextureManager.GetTexture( self.north ).bind()
        self.drawQuad( nwt, net, nwb, neb )

        #draw the south face
        TextureManager.GetTexture( self.south ).bind()
        self.drawQuad(  set, swt, seb, swb )

        #draw the east face
        TextureManager.GetTexture( self.east ).bind()
        self.drawQuad(  net, set, neb, seb )

        #draw the west face
        TextureManager.GetTexture( self.west ).bind()
        self.drawQuad(  swt, nwt, swb, nwb )

        #draw the top face
        TextureManager.GetTexture( self.up ).bind()
        self.drawQuad(  swt, set, nwt, net )

        glPopMatrix()

        glEnable(GL_DEPTH_TEST)
        glEnable(GL_CULL_FACE)

class Atmosphere(object):
    def __init__(self, project_desc ):
        self.fogStart = float(project_desc.FogStart)
        self.fogEnd   = float(project_desc.FogEnd)
        self.viewDistance = float( project_desc.ViewDistance )
        r = float( project_desc.FogColor['r'] ) / 255.0
        g = float( project_desc.FogColor['g'] ) / 255.0
        b = float( project_desc.FogColor['b'] ) / 255.0
        a = float( project_desc.FogColor['a'] ) / 255.0
        self.fogColor = a,r,g,b
        if project_desc.DrawSkyBox.lower() == 'true':
            self.skyBox = SkyBox(project_desc.North,
                                project_desc.South,
                                project_desc.East,
                                project_desc.West,
                                project_desc.Up,
                                project_desc.Down)
        else:
            self.skyBox = None

    def setUnderwaterState(self):
        a,r,g,b = 1, 0.1, 0.4, 0.7#self.fogColor
        GraphicsCard.clearColor( (r, g, b, a) )
        glFogi( GL_FOG_MODE, GL_LINEAR )
        glFogfv( GL_FOG_COLOR, [r,g,b,a] )
        glFogf( GL_FOG_DENSITY, 1.0 )
        glHint( GL_FOG_HINT, GL_DONT_CARE )
        glFogf( GL_FOG_START, 10)
        glFogf( GL_FOG_END,  600)
        glEnable(GL_FOG)
        GraphicsCard.setScreenProjection( float(GraphicsCard.width/GraphicsCard.height), 0.1, 600)

    def setState(self):
        a,r,g,b = self.fogColor
        GraphicsCard.clearColor((r, g, b, a))
        glFogi( GL_FOG_MODE, GL_LINEAR )
        glFogfv( GL_FOG_COLOR, [r,g,b,a] )
        glFogf( GL_FOG_DENSITY, 0.35 )
        glHint( GL_FOG_HINT, GL_DONT_CARE )
        glFogf( GL_FOG_START, float(self.fogStart) )
        glFogf( GL_FOG_END,  float(self.fogEnd) )
        glEnable(GL_FOG)

        #set the projection matrix
        #glMatrixMode(GL_PROJECTION)
        #glLoadIdentity()
        #gluPerspective(60.0, float(Graphics.width)/float(Graphics.height), 0.1, self.viewDistance)
        #glMatrixMode(GL_MODELVIEW)
        GraphicsCard.setScreenProjection( float(GraphicsCard.width/GraphicsCard.height), 0.1, self.viewDistance )

    def draw(self, camera):
        if self.skyBox:
            self.skyBox.draw(camera)
