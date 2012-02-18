# I M P O R T S ##################################################
import encodings.ascii #for cxfreeze
import gc
import os
import sys
import math
import time
import pygame
import OpenGL.GL
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from cgkit.cgtypes import *
from MathUtil import *

import Graphics
import TextureManager
import Camera
import gl2D
import Font
import Terrain
import Heightmap
import Shader
import World
import TerrainMaterial
import Settings
import DebugDisplay
import graphext
import Console
import FontManager

from network.Message import MessageStruct

from Platform import time_in_seconds, isUnix
from Graphics import GraphicsCard
from Client import GameClient

# C L A S S E S ##################################################


class MainApp(object):
    """The main app class"""

    def __init__(self):
        self.consoleVisible = False

    def load(self):
        self.keyDown   = {}
        self.fps       = 0
        self.right_btn = False
        #initialize the screen/window
        self.size = self.width, self.height = Settings.Resolution
        self.fullscreen = Settings.Fullscreen
        self.resetVideoMode()

        #init network variable thingy
        self.network = None

        #create console
        self.console = Console.Console( Graphics.Rectangle( 20, 20, self.width-20, self.height-20) )
        self.console.setAsStdOut()
        self.console.onInput = self.onConsoleInput

        #initialize some basic opengl states
        GraphicsCard.enable( 'depth_test')
        GraphicsCard.setDepthFunc('less')
        GraphicsCard.setShadeModel( 'smooth' )
        GraphicsCard.setScreenProjection( float(self.width/self.height), 0.1, 5000.0 )

        self.font    = Font.TextureFont( '../base/fonts/tahoma.fnt' )
        self.bigfont = Font.TextureFont( '../base/fonts/tahoma_20.fnt' )
        FontManager.GetFont( Settings.ConsoleFont )
        loadscreen = TextureManager.GetTexture( '../base/art/ui/facehatlogo.png' )

        #Draw loading screen
        GraphicsCard.clearDepth(1.0)
        GraphicsCard.clearColor( (1, 1, 1, 1) )
        GraphicsCard.clear()
        #glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
        GraphicsCard.setBlendFunction( 'src_alpha', 'one_minus_src_alpha' )
        GraphicsCard.enable( 'blend', 'texture_2d')
        gl2D.start2D()
        gl2D.drawTexture2D( loadscreen, self.width/2-256, self.height/2-256, 512, 512 )
        self.bigfont.draw(self.width/2, self.height/2-280, "Loading..." )
        gl2D.end2D()
        pygame.display.flip()

        #hide/show cursor
        if Settings.GrabMouse:
            pygame.mouse.set_visible(False)
            pygame.event.set_grab(True)
        else:
            pygame.mouse.set_visible(True)
            pygame.event.set_grab(False)

        #Initialize OpenGL extensions
        GraphicsCard.initExtensions()

        #check for OpenGL 2.0
        if Settings.UseShaders and GraphicsCard.hasShaders():
            print "Shader Support Present"
            Settings.UseShaders = True
        else:
            print "Warning: No shader support, or shaders disabled"
            Settings.UseShaders = False
        print "Max Anisotropy:", GraphicsCard.getMaxAnisotropy()

        ###load the map
        if Settings.SinglePlayer:
            self.world = World.World( Settings.DefaultMod, Settings.DefaultMap, is_server=True, graphics_enabled=True )
        else:
            self.world = World.World( Settings.DefaultMod, Settings.DefaultMap, is_server=False, graphics_enabled=True )
        self.world.initGraphics()
        self.lastjump = time_in_seconds()

        #setup lighting
        n = vec3(0, 1, 0 )
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0] )
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, [0.9, 0.9, 0.9, 1.0] )
        glLightfv( GL_LIGHT0, GL_AMBIENT,  [0.0, 0.0, 0.0, 1.0 ] )
        glLightfv( GL_LIGHT0, GL_DIFFUSE,  [1.0, 1.0, 1.0, 1.0 ] )
        glLightfv( GL_LIGHT0, GL_POSITION, [n.x, n.y, n.z, 0] )
        GraphicsCard.clearColor((0.0, 0.0, 1.0, 0.0))
        GraphicsCard.enable('light0')

    def connect(self, address):
        """Connects to a remote game server"""
        print "Connecting to", address
        self.network = GameClient(address, 1337, self.world, Settings.Name)

    def onConsoleInput(self, line):
        def help(something):
            print "Help feature currently disabled due to it not working properly with in-game console"
        try:
            exec line
        except Exception, e:
            print e

    def toggleExclusiveInputMode(self):
        Settings.GrabMouse = not Settings.GrabMouse
        if Settings.GrabMouse:
            pygame.mouse.set_visible(False)
            pygame.event.set_grab(True)
        else:
            pygame.mouse.set_visible(True)
            pygame.event.set_grab(False)

    def toggleConsole(self):
        self.consoleVisible = not self.consoleVisible
        if self.consoleVisible:
            pygame.key.set_repeat( 200, 50 )
        else:
            pygame.key.set_repeat( )

    def resetVideoMode(self):
        flags = pygame.OPENGL | pygame.DOUBLEBUF
        if self.fullscreen: flags |= pygame.FULLSCREEN
        self.screen = pygame.display.set_mode(self.size, flags )
        self.size = self.width, self.height = GraphicsCard.getScreenSize()

    def handleKeyDownEvent(self, event ):
        """Store the key state in the dictionary"""
        self.keyDown[event.key] = True
        if event.unicode == '`':
            return self.toggleConsole()
        if self.consoleVisible:
            self.console.onKeyPressed(event)

    def handleKeyUpEvent( self, event ):
        """Store the key state in the dictionary (or exit)"""
        self.handleKeyPress( event )
        self.keyDown[event.key] = False

    def handleKeyPress( self, event ):
        if event.key == pygame.K_ESCAPE:
            sys.exit()
        if event.key == pygame.K_F1:
            Settings.DrawDebugInfo = not Settings.DrawDebugInfo
        if event.key == pygame.K_F2:
            Settings.RunAI = not Settings.RunAI
        if event.key == pygame.K_F3:
            self.connect("localhost")
        if event.key == pygame.K_F4:
            Settings.RunPhysics = not Settings.RunPhysics
        if event.key == pygame.K_F5:
            self.toggleExclusiveInputMode()
        if event.key == pygame.K_F6:
            Settings.DrawViewFrustrums = not Settings.DrawViewFrustrums
        if event.key == pygame.K_F7:
            Settings.DrawPaths = not Settings.DrawPaths
        if event.key == pygame.K_F8:
            Settings.DebugTransforms = not Settings.DebugTransforms

    def _isKeyDown( self, key ):
        """Checks if the specified key is down"""
        if not key in self.keyDown:
            self.keyDown[key] = False
        return self.keyDown[key]

    def update(self, timedelta):
        """Updates the game state"""
        if timedelta:
            self.fps = 1.0/timedelta
        cameraMoveVector = vec3(0,0,0)
        jump_force = 0
        mx, my = 0, 0
        if Settings.RunPhysics:
            move_speed = 100
            max_jump_force = 200
        else:
            move_speed = 5
            max_jump_force = 5
        if isinstance( self.world.camera, Camera.Freecam ):
            move_speed = 5
            max_jump_force = 5

        if not self.consoleVisible:
            if self._isKeyDown(pygame.K_LEFT): mx = -6
            if self._isKeyDown(pygame.K_RIGHT): mx = 6
            if self._isKeyDown(pygame.K_UP): my = -6
            if self._isKeyDown(pygame.K_DOWN): my = 6
            if self._isKeyDown(pygame.K_w): cameraMoveVector.z = 1
            if self._isKeyDown(pygame.K_s): cameraMoveVector.z = -1
            if self._isKeyDown(pygame.K_a): cameraMoveVector.x = -1
            if self._isKeyDown(pygame.K_d): cameraMoveVector.x =  1
            if self._isKeyDown(pygame.K_x): self.world.frustrum_camera = self.world.camera.asFreecam()
            if self._isKeyDown(pygame.K_SPACE):
                #if time_in_seconds() - self.lastjump > 0.5:
                jump_force = max_jump_force
                self.lastjump = time_in_seconds()
            if cameraMoveVector.length():
                cameraMoveVector = cameraMoveVector.normalize()
                cameraMoveVector = cameraMoveVector * move_speed

            left_btn, middle_btn, right_btn = pygame.mouse.get_pressed()

            if right_btn or Settings.GrabMouse:
                mx, my = pygame.mouse.get_rel()
            else:
                pygame.mouse.get_rel()
            self.right_btn = right_btn
            self.world.camera.turn( degreeToRadian( -mx*0.2 ),
                                    degreeToRadian( -my*0.2 ),
                                    0 )

        #move the camera
        cameraMoveVector.y = jump_force
        self.world.camera.move( cameraMoveVector )

        px = self.world.camera.position.x
        py = self.world.camera.position.y
        pz = self.world.camera.position.z
        yaw = self.world.camera.yaw
        DebugDisplay.update('world_position', (int(px), int(py), int(pz)) )
        DebugDisplay.update('opacity',
                            self.world.lightIntensityAt( px, pz) )

        #update world
        self.world.update( timedelta )

        if self.network: self.network.update()

        #update debug display
        DebugDisplay.step( timedelta )

    def drawScreen( self ):
        """Draw the screen"""
        GraphicsCard.resetPolygonCount()
        GraphicsCard.enable('texture_2d', 'blend', 'cull_face')
        GraphicsCard.setBlendFunction('src_alpha', 'one_minus_src_alpha' )
        GraphicsCard.setFrontFace('ccw')

        GraphicsCard.loadIdentity()
        GraphicsCard.multMatrix( self.world.camera.getViewMatrix().toList() )
        glLightfv( GL_LIGHT0, GL_POSITION, [0, 1, 0, 0] )
        GraphicsCard.clearDepth(1.0)
        GraphicsCard.clear()

        #Draw the world
        self.world.draw()

        TextureManager.DisableStage( 1 )
        TextureManager.DisableStage( 2 )
        if Settings.UseShaders: Shader.DisableShaders()
        #self.font.draw( 0, 0, "FPS: " + str( self.fps ) )
        DebugDisplay.update("FPS", self.fps, delay=0.3333 )
        DebugDisplay.update("yaw", radianToDegree( self.world.camera.yaw) )
        DebugDisplay.update("pitch", radianToDegree( self.world.camera.pitch ))
        DebugDisplay.update("polygons_drawn", GraphicsCard.polygonsDrawn() )
        if self.world.playerEnt:
            DebugDisplay.update("onGround", self.world.playerEnt.onGround )
            DebugDisplay.update("groundNormal", self.world.playerEnt.groundNormal )


        GraphicsCard.loadIdentity()
        if self.consoleVisible:
            self.console.draw()
        gl2D.start2D()
        if Settings.DrawDebugInfo:
            GraphicsCard.setColorRGBA(1, 1, 1, 1)
            self.font.draw(0, 0, str(DebugDisplay.globalState))
            self.world.scene.drawDebugInfo(self.font)
        else:
            GraphicsCard.setColorRGBA(1, 1, 1, 1)
            pygame.display.set_caption("FPS: " + str(DebugDisplay.get("FPS")))
            #self.bigfont.draw(0, 0, "Ninjas  Killed: %s" % (self.world.ninjasKilled) )
            #self.bigfont.draw(0, 30, "Pirates Killed: %s" % (self.world.piratesKilled) )
            #self.bigfont.draw(0, 60, "Treasures Stolen: %s" % (self.world.treasuresTaken) )
        #DebugDisplay.globalState.draw( self.font )
        gl2D.end2D()
        #put stuff on the screen
        pygame.display.flip()

        err = graphext.GetGLError()
        while err != GL_NO_ERROR:
            print err
            print graphext.GLErrorString( err)


def main():
    #initialize pygame
    pygame.init()

    #Profile the loading
    start_time = time.time()
    app = MainApp()
    if Settings.ProfileLoad:
        load_prof = hotshot.Profile('load_profile')
        load_prof.runcall(app.load)
    else:
        app.load()
    gc.collect()
    print "Load Time: %s" % ( str( time.time() - start_time ))
    if Settings.RunProfiler:
        run_prof = hotshot.Profile('run_profile')
        run_prof.runcall( mainloop, app)
    mainloop(app)

def mainloop(app):
    time_delta = 0.00001
    last_tick = 0
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
            elif event.type == pygame.KEYDOWN: app.handleKeyDownEvent( event )
            elif event.type == pygame.KEYUP: app.handleKeyUpEvent( event )
        last_tick  = time_in_seconds()
        app.update(time_delta)
        app.drawScreen()
        time_delta = time_in_seconds() - last_tick
    del app



#run the main function if this is the main python file invoked
if __name__ == '__main__':
    if Settings.RunProfiler or Settings.ProfileLoad:
        import hotshot
    # Import Psyco if available
    if not __debug__:
        print "Running in release mode"
        if Settings.Optimize and not isUnix(): #doesn't run psyco in linux (segfaults for some reason)
            try:
                import psyco
                psyco.full(memory=100)
                print "Using optimized code"
            except ImportError:
                print "[WARNING:] Optimizations not present"
    else:
        print "Running in debug mode"
    main()
