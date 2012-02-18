#imports
from OpenGL.GL import *
import array

#global variables (dios mio!)
_screen_width  = 0
_screen_height = 0
_stack_count = 0

def screenSize():
    global _screen_width, _screen_height
    return _screen_width, _screen_height

def start2D():
    global _screen_width
    global _screen_height
    global _stack_count
    if _stack_count > 0:
        _stack_count += 1
        return
    _stack_count += 1

    x,y,_screen_width,_screen_height = glGetDoublev( GL_VIEWPORT )
    _screen_width  = int( _screen_width )
    _screen_height = int( _screen_height )
    #print x,y,_screen_width,_screen_height
    glMatrixMode( GL_PROJECTION )
    glPushMatrix()
    glLoadIdentity()
    glOrtho( 0, _screen_width, 0, _screen_height, -1, 1 )
    glMatrixMode( GL_MODELVIEW )

    glDisable( GL_DEPTH_TEST )


def end2D( ):
    global _stack_count
    _stack_count -= 1
    if _stack_count > 0:
        return

    glMatrixMode( GL_PROJECTION )
    glPopMatrix()
    glMatrixMode( GL_MODELVIEW )
    glEnable( GL_DEPTH_TEST )



def drawTexture2D( texture, x, y, w, h, u1=0, v1=0, u2=1, v2=1  ):
    global _screen_width
    global _screen_height
    glPushMatrix()
    y = _screen_height-y
    glTranslatef( x, y, 0 )

    if texture: texture.bind()
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

def drawSubTexture2D( texture, dx, dy, sx, sy, sw, sh ) :
    width = texture.width
    height = texture.height
    u = float(sx)/width
    v = float(sy)/height
    u2 = float((sx+sw))/width
    v2 = float((sy+sh))/height

    drawTexture2D( texture, dx, dy, sw, sh, u, v, u2, v2 )

