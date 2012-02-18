# I M P O R T S ##################################################
import sys
import math
import time
import pygame
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import glew
from cgkit.cgtypes import *
from MathUtil import *

import TextureManager
import Camera
import gl2D
import Font
from Graphics import GraphicsCard


# C L A S S E S ##################################################

class GlTestApp(object):
    """The main app class"""
    move_speed=1

    def __init__(self):
        #initialize the screen/window
        self.size = self.width, self.height = 1024, 768
        self.screen = pygame.display.set_mode(self.size, pygame.OPENGL |
                                                         pygame.DOUBLEBUF ) # |
                                                         #pygame.FULLSCREEN )
        glew.glewInit()

        #check for OpenGL 2.0
        if glew.GLEW_ARB_vertex_shader and glew.GLEW_ARB_fragment_shader:
            print "Shader Support Present"

        #initialize some basic opengl states
        glDepthFunc(GL_LESS)
        glEnable(GL_DEPTH_TEST)
        glShadeModel(GL_SMOOTH)

        #set the projection matrix
        GraphicsCard.setScreenProjection( float(self.width/self.height), 0.1, 5000.0 )
        #glMatrixMode(GL_PROJECTION)
        #glLoadIdentity()
        #gluPerspective(60.0, float(self.width)/float(self.height), 0.1, 1000.0)

        #self.projection = glGetDoublev( GL_PROJECTION_MATRIX )
        self.projection = GraphicsCard.screenProjection

        self.keyDown = {}

        self.font = Font.TextureFont( '../base/fonts/tahoma.fnt' )
        self.fps = 0

        #set camera
        self.camera = Camera.Freecam()
        self.camera.position = vec3( 0.0, 0.0, 1.5 )
        self.camera.yaw = degreeToRadian( 0 )
        self.camera.pitch = degreeToRadian( 0 )
        self.right_btn = False

        #return to modelview
        glMatrixMode(GL_MODELVIEW)
        print "Base Init OK!"
        self.onInit()


    def onInit( self ):
        pass

    def setRepeatRate( self, delay, interval ):
        return pygame.key.set_repeat( delay, interval)

    def handleKeyDownEvent(self, event ):
        #Store the key state in the dictionary
        self.keyDown[event.key] = True
        self.onKeyDown(event)

    def handleKeyUpEvent( self, event ):
        #Store the key state in the dictionary (or exit)
        if event.key == pygame.K_ESCAPE:
            sys.exit()

        self.keyDown[event.key] = False
        self.onKeyUp(event)


    """Checks if the specified key is down"""
    def _isKeyDown( self, key ):
        if self.keyDown.has_key( key ) == False:
            self.keyDown[key] = False
        return self.keyDown[key]

    """Updates the game state"""
    def update(self, timedelta):
        if timedelta:
            self.fps = 1.0/timedelta
        cameraMoveVector = vec3(0,0,0)
        move_speed = self.move_speed

        if self._isKeyDown(pygame.K_w): cameraMoveVector.z =  1
        if self._isKeyDown(pygame.K_s): cameraMoveVector.z = -1
        if self._isKeyDown(pygame.K_a): cameraMoveVector.x = -1
        if self._isKeyDown(pygame.K_d): cameraMoveVector.x =  1
        if self._isKeyDown(pygame.K_SPACE): cameraMoveVector.y =  1
        if self._isKeyDown(pygame.K_LCTRL): cameraMoveVector.y = -1

        left_btn, middle_btn, right_btn = pygame.mouse.get_pressed()
        if right_btn:
            mx, my = pygame.mouse.get_rel()
            self.camera.yaw   += degreeToRadian(-mx * 0.2)
            self.camera.pitch += degreeToRadian(-my * 0.2)
            self.right_btn = right_btn
        else:
            self.right_btn = right_btn
            pygame.mouse.get_rel()

        #move the camera
        self.camera.move( cameraMoveVector*timedelta*move_speed )
        self.camera.update(timedelta)
        self.onUpdate( timedelta )

    def onUpdate( self, timedelta):
        """Override me!"""
        pass

    def onKeyDown(self, key):
        pass

    def onKeyUp( self, key ):
        pass

    def drawScreen( self ):
        glEnable( GL_TEXTURE_2D )
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)
        glClearColor(0.0, 0.0, 1.0, 0.0)
        glClearDepth(1.0)

        glLoadIdentity()
        #glLoadMatrixf( self.camera.getViewMatrix().toList() )

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.onDraw( )
        glLoadIdentity()
        self.font.draw( 0, 0, "FPS: " + str( self.fps ) )

        #put stuff on the screen
        pygame.display.flip()

    def onDraw( self ):
        pass


def run( main_class ):
    #initialize pygame
    pygame.init()
    app = main_class()

    #run an infinite loop
    time_delta = 0.0
    last_tick = 0
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
            elif event.type == pygame.KEYDOWN: app.handleKeyDownEvent( event )
            elif event.type == pygame.KEYUP: app.handleKeyUpEvent( event )
        app.update(time_delta)
        app.drawScreen()
        time_delta = time.clock() - last_tick
        last_tick  = time.clock()




#run the main function if this is the main python file invoked
if __name__ == '__main__':
    run( GlTestApp)
