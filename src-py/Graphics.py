import OpenGL.GL
import math
import string
import glew
from OpenGL.GL import *
from OpenGL.GLU import *
from cgkit.cgtypes import *
import ListFunctions
from DisplayList import DisplayList
import gl2D

OpenGLStateMap = {
    'blend' : GL_BLEND,
    'lighting'   : GL_LIGHTING,
    'fog'        : GL_FOG,
    'texture_2d' : GL_TEXTURE_2D,
    'alpha_test' : GL_ALPHA_TEST,
    'depth_test' : GL_DEPTH_TEST
}

OpenGLBlendMap = {
    'zero': GL_ZERO,
    'one' : GL_ONE,
    'src_alpha' : GL_SRC_ALPHA,
    'one_minus_src_alpha' : GL_ONE_MINUS_SRC_ALPHA,
    'src_color' : GL_SRC_COLOR,
    'dst_color' : GL_DST_COLOR,
    'one_minus_src_color' : GL_ONE_MINUS_SRC_COLOR,
    'one_minus_dst_color' : GL_ONE_MINUS_DST_COLOR,
    'dst_alpha' : GL_DST_ALPHA,
    'one_minus_dst_alpha' : GL_ONE_MINUS_DST_ALPHA,
    'src_alpha_saturate' : GL_SRC_ALPHA_SATURATE
}

def GetOpenGLConstantFromString(const_str):
    return getattr(OpenGL.GL, 'GL_' + string.upper( const_str ) )

class Rectangle(object):
    def __init__(self, left=0, top=0, right=0, bottom=0):
        self.left   = left
        self.top    = top
        self.right  = right
        self.bottom = bottom

    def _getwidth(self): return self.right - self.left
    def _setwidth(self, w): self.right = self.left + w
    def _getheight(self): return self.bottom - self.top
    def _setheight(self, h): self.bottom = self.top + h
    def _getx(self): return self.left
    def _gety(self): return self.top
    def _setx(self, x):
        w = self.width
        self.left = x
        self.right = x + w
    def _sety(self, y):
        h = self.height
        self.top = y
        self.bottom = y + h

    x = property(_getx)
    y = property(_gety)
    width = property(_getwidth, _setwidth)
    height = property(_getheight, _setheight)

    def draw(self, r, g, b, a ):
        glColor4f( r, g, b, a )
        GraphicsCard.drawRectangle( self )


class Projection(object):
    def __init__(self, *args, **kwargs):
        self.update( *args, **kwargs)

    def update(self, aspect, near, far, fieldOfViewDegrees):
        self.aspect = float(aspect)
        self.near = near
        self.far = far
        self.FOV = fieldOfViewDegrees
        self._constructProjectionMatrix()
        #self.getProjectionMatrixFromGL()

    def _constructProjectionMatrix(self):
        f = 1.0 / math.tan( self.FOV / 2.0 )
        a = self.aspect
        zn = self.near
        zf = self.far
        zn_zf = zn - zf
        matrix = [f/a,   0,         0,           0,
                  0,     f,         0,           0,
                  0,     0,   (zf+zn)/zn_zf, (2*zf*zn)/zn_zf,
                  0,     0,        -1,           0 ]
        self.projectionMatrix = mat4(matrix)
        return self.projectionMatrix


    def _getProjectionMatrixFromGL(self ):
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPerspective(self.FOV, self.aspect, self.near, self.far)
        self.projectionMatrix = mat4(
            ListFunctions.flattenListOfLists(
                glGetDoublev( GL_PROJECTION_MATRIX ))
            ).transpose()
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW)
        return self.projectionMatrix

    def set(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(self.FOV, self.aspect, self.near, self.far)
        glMatrixMode(GL_MODELVIEW)

class FogState(object):
    def __init__(self, color_tuple_argb, density, start_distance, end_distance):
        self.color = color_tuple_argb
        self.density = density
        self.startDistance = start_distance
        self.endDistance = end_distance

    def bind(self):
        a,r,g,b = self.color
        GraphicsCard.clearColor((r, g, b, a))
        glFogi( GL_FOG_MODE, GL_LINEAR )
        glFogfv( GL_FOG_COLOR, [r,g,b,a] )
        glFogf( GL_FOG_DENSITY, self.density )
        glHint( GL_FOG_HINT, GL_DONT_CARE )
        glFogf( GL_FOG_START, float(self.startDistance) )
        glFogf( GL_FOG_END,  float(self.endDistance) )
        glEnable(GL_FOG)

class GlewInitError(Exception): pass

class GraphicsCard(object):
    polycount = 0
    width  = 0
    height = 0
    screenProjection = None
    constantMap = {} #maps a string like 'blend' to the constant for GL_BLEND
    glewVersion = None

    @classmethod
    def initExtensions(cls):
        err = glew.glewInit()
        if err != glew.GLEW_OK:
            raise GlewInitError( glew.glewGetErrorString(err) )
        cls.glewVersion = glew.glewGetString( glew.GLEW_VERSION )

    @staticmethod
    def hasShaders():
        return (glew.GLEW_ARB_vertex_shader() and glew.GLEW_ARB_fragment_shader())

    @staticmethod
    def clearDepth(depth):
        glClearDepth( depth )

    @staticmethod
    def clearColor(color_tuple_rgba):
        glClearColor(*color_tuple_rgba)

    @staticmethod
    def clear(colors=True, depthbuffer=True):
        flags = 0
        if colors: flags |= GL_COLOR_BUFFER_BIT
        if depthbuffer: flags |=  GL_DEPTH_BUFFER_BIT
        glClear(flags)

    @classmethod
    def setScreenProjection(cls, aspect, near, far, fieldOfViewDegrees=60.0):
        cls.screenProjection = Projection(aspect, near, far, fieldOfViewDegrees)
        cls.screenProjection.set()

    @classmethod
    def getProjectionMatrix(cls):
        return cls.screenProjection.projectionMatrix

    @classmethod
    def polygonsDrawn(cls):
        return cls.polycount

    @classmethod
    def resetPolygonCount(cls):
        cls.polycount = 0

    @classmethod
    def recordDraw(cls, polygons_drawn):
        cls.polycount += polygons_drawn

    @classmethod
    def getScreenSize(cls):
        x,y,cls.width,cls.height = glGetFloatv( GL_VIEWPORT )
        return cls.width, cls.height

    @classmethod
    def getScreenRectangle(cls):
        x,y,cls.width,cls.height = glGetFloatv( GL_VIEWPORT )
        return Rectangle(x, y, x+cls.width, y+ cls.height)

    @classmethod
    def projectWorldPointToScreen(cls, x, y, z, camera_matrix=None):
        if not camera_matrix:
            glGetFloatv( GL_MODELVIEW_MATRIX )
        wx, wy, wz = gluProject(x, y, z,
                          camera_matrix.toList(),
                          cls.screenProjection.projectionMatrix.toList(),
                          [0, 0, cls.width, cls.height] )
        wy = cls.height - wy - 1
        return wx, wy, wz

    @staticmethod
    def start2D():
        gl2D.start2D()

    @staticmethod
    def end2D():
        gl2D.end2D()

    @staticmethod
    def loadIdentity(): glLoadIdentity()

    @staticmethod
    def pushMatrix(): glPushMatrix()

    @staticmethod
    def popMatrix(): glPopMatrix()

    @staticmethod
    def translate( *args ):
        if len(args) == 1:
            x,y,z = args[0], args[1], args[2]
            glTranslatef(x,y,z)
        else:
            glTranslatef(*args)

    @staticmethod
    def rotate( angle, x, y, z ): glRotatef( angle, x, y, z )

    @staticmethod
    def multMatrix(m):
        if type(m) is list or type(m) is tuple:
            glMultMatrixf(m)
        elif type(m) is mat4:
            glMultMatrixf(m.toList())

    @classmethod
    def getGLConstant(cls, const_str ):
        try:
            return cls.constantMap[const_str]
        except KeyError:
            cls.constantMap[const_str] = GetOpenGLConstantFromString(const_str)
        return cls.constantMap[const_str]

    @classmethod
    def switchState(cls, state, aBoolean):
        state_constant = cls.getGLConstant(state)
        if aBoolean:
            glEnable(state_constant)
        else:
            glDisable(state_constant)

    @classmethod
    def switchStates(cls, state_list, b):
        for state in state_list:
            cls.switchState( state, b)

    @classmethod
    def enable(cls, *args): cls.switchStates(args, True)

    @classmethod
    def disable(cls, *args): cls.switchStates(args, False)

    @classmethod
    def setFrontFace(cls, frontface):
        ffc = cls.getGLConstant(frontface)
        glFrontFace( ffc )

    @classmethod
    def setBlendFunction(cls, source, dest):
        srcconst =  cls.getGLConstant(source)
        destconst =  cls.getGLConstant(dest)
        glBlendFunc( srcconst, destconst )

    @classmethod
    def setAlphaFunc( cls, func, param):
        funcc = cls.getGLConstant( func )
        glAlphaFunc( funcc, param )

    @classmethod
    def setDepthFunc(cls, depth_func):
        func_constant = cls.getGLConstant(depth_func)
        glDepthFunc(func_constant)

    @classmethod
    def setShadeModel(cls, shademodel):
        sm = cls.getGLConstant(shademodel)
        glShadeModel(sm)

    @staticmethod
    def enableTexture( aBoolean ):
        if aBoolean:
            glEnable(GL_TEXTURE_2D)
        else:
            glDisable(GL_TEXTURE_2D)

    @staticmethod
    def enableDepthTest( aBoolean):
        if aBoolean:
            glEnable(GL_DEPTH_TEST)
        else:
            glDisable(GL_DEPTH_TEST)

    @staticmethod
    def enableAlphaTest( aBoolean):
        if aBoolean:
            glEnable(GL_ALPHA_TEST)
        else:
            glDisable(GL_ALPHA_TEST)

    @staticmethod
    def setColorRGBA( r, g, b, a=1.0):
        glColor4f( r, g, b, a)

    @staticmethod
    def enableAlphaBlending( aBoolean):
        if aBoolean:
            glEnable( GL_BLEND )
        else:
            glDisable(GL_BLEND)

    #utility methods
    @classmethod
    def getMaxAnisotropy(cls):
        #return glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
        return 8.0

    @staticmethod
    def getTextureAnisotropy():
        return glGetTexParameterfv(GL_TEXTURE_2D, glew.GL_TEXTURE_MAX_ANISOTROPY_EXT)
        #return 8.0

    @staticmethod
    def setTextureAnisotropy( anisotropy):
        glTexParameterf(GL_TEXTURE_2D, glew.GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy)

    @classmethod
    def drawQuad(cls, rectangle, u1=0, v1=0, u2=1, v2=1 ):
        sw, sh = cls.getScreenSize()
        x = rectangle.x
        y = sh-rectangle.y
        w = rectangle.width
        h = rectangle.height
        glPushMatrix()
        glTranslatef( x, y, 0 )
        glBegin(GL_QUADS)
        glTexCoord2f( u1, v2 )
        glVertex3f( 0, -h, -.1 )
        glTexCoord2f( u2, v2 )
        glVertex3f(  w, -h, -.1 )
        glTexCoord2f( u2, v1 )
        glVertex3f(  w, 0, -.1 )
        glTexCoord2f( u1, v1 )
        glVertex3f( 0, 0, -.1 )
        glEnd()
        glPopMatrix()

def drawWirePyramid( width=1.0, height=1.0,  depth=1.0 ):
    start_color = 0,0,0,1
    end_color   = 0,0,0,0.25
    glLineWidth( 3 )
    glDisable(GL_LIGHTING)
    glDisable(GL_TEXTURE_2D)
    glEnable( GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)

    #Draw faces that define the pyramid
    glBegin( GL_LINES )

    glColor4f( *start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *end_color)
    glVertex3f( -width, height, -depth )
    glColor4f( *start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *end_color)
    glVertex3f(  width, height, -depth )
    glColor4f( *start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *end_color)
    glVertex3f( -width, -height, -depth )
    glColor4f( *start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *end_color)
    glVertex3f(  width, -height, -depth )

    glColor4f( *end_color)
    glVertex3f( -width, -height, -depth )
    glVertex3f(  width, -height, -depth )

    glVertex3f(  width, -height, -depth )
    glVertex3f(  width,  height, -depth )

    glVertex3f(  width,  height, -depth )
    glVertex3f( -width,  height, -depth )

    glVertex3f(  -width, height, -depth )
    glVertex3f(  -width, -height, -depth )


    glEnd()
    glLineWidth( 1 )
    glEnable(GL_TEXTURE_2D)

def drawPyramid( width=1.0, height=1.0,  depth=1.0 ):
    glPushAttrib( GL_LIGHTING)
    glPushAttrib( GL_CULL_FACE)
    glPushAttrib( GL_TEXTURE_2D)
    glDisable(GL_TEXTURE_2D)
    glDisable(GL_CULL_FACE)
    #glCullFace(GL_CW)
    glDisable(GL_LIGHTING)
    glEnable( GL_BLEND)
    #glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
    glBlendFunc(GL_ONE,GL_ONE)
    glDisable( GL_ALPHA_TEST)
    glDepthMask( GL_FALSE )
    side_start_color = 0.35, 0.35, 0.35, .5
    side_end_color   = 0, 0, 0, 0
    top_start_color = 0.35, 0.35, 0.35, .5
    top_end_color   = 0, 0, 0, 0

    #Draw faces that define the pyramid
    glBegin( GL_TRIANGLES )

    #bottom plane
    glColor4f( *top_start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *top_end_color)
    glVertex3f( width, -height, -depth )
    glVertex3f( -width, -height, -depth )

    #top plane
    glColor4f( *top_start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *top_end_color)
    glVertex3f( -width, height, -depth )
    glVertex3f(  width, height, -depth )

    #right plane
    glColor4f( *side_start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *side_end_color)
    glVertex3f(  width,  height, -depth )
    glVertex3f(  width, -height, -depth )

    #left plane
    glColor4f( *side_start_color)
    glVertex3f(  0, 0, 0 )
    glColor4f( *side_end_color)
    glVertex3f( -width, -height, -depth )
    glVertex3f( -width,  height, -depth )

    glEnd()

    glPopAttrib()
    glPopAttrib()
    glPopAttrib()
    glDepthMask( GL_TRUE )


_sphere = None
_circleQuad = None
def _makeSphere():
    global _sphere
    global _circleQuad
    _circleQuad   = gluNewQuadric()
    _sphere = DisplayList( (lambda: gluSphere( _circleQuad, 1.0, 12, 12 )) )


class Path(object):
    def __init__(self, points, radius, linewidth):
        if not _sphere: _makeSphere()
        self.displayList = DisplayList( lambda: drawPath(points, radius, linewidth) )

    def draw(self):
        self.displayList()




def drawSphere(point, rx=1, ry=1, rz=1):
    if not _sphere: _makeSphere()
    glPushMatrix()
    glTranslatef( point[0], point[1], point[2] )
    glScalef(rx, ry, rz )
    #gluSphere( _circleQuad, 1.0, 12, 12 )
    _sphere()
    glPopMatrix()

def drawPath(points, radius, linewidth):

    glDisable(GL_TEXTURE_2D)
    glDisable(GL_LIGHTING)
    glDisable( GL_ALPHA_TEST)
    glEnable( GL_BLEND)
    glEnable(GL_CULL_FACE)
    glBlendFunc(GL_ONE,GL_ONE)

    #glDepthMask( GL_FALSE )
    #draw a circle for each point
    glColor4( 0.5, 0.5, 0.5, 1)
    drawpoints = [x+vec3(0,5,0) for x in points]
    for p in drawpoints:
        drawSphere( p, radius, radius, radius)
    #glDisable(GL_BLEND)
    #glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
    glColor4(0,0.0,1,0.5)
    glLineWidth( linewidth)
    glBegin(GL_LINE_STRIP)
    for p in drawpoints:
        glVertex3f( *p )
    glEnd()
    glLineWidth( 1.0 )
    #glDisable(GL_BLEND)
    #glDisable(GL_CULL_FACE)
