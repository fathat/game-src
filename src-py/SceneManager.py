import MeshLoader
import Mod
import elementtree.ElementTree as ET
import string
import ConfigParser
import TextUtil
import MathUtil
import Ents
import EntLoader
import mathhat
import DebugDisplay
from StaticMeshScene import StaticMeshScene
from GameProtocol import ByteToRadian
from OpenGL.GL  import *
from OpenGL.GLU import *
from cgkit.cgtypes import *

class Message(object):
    def __init__(self, notify=None, type=None):
        """Initializes a message and specifies who to notify. If
        None is specified, notify everyone"""


class SceneManager(object):
    """Takes care of objects in the world"""

    def __init__(self, world, modname, mapname ):
        """Load the objects from the map"""

        #store instance variables
        self.world = world

        #Load from the XML file holding the entities
        filename = Mod.MapPath(modname, mapname) + "objects.mb2"
        self.ents = EntLoader.LoadEntsFromFile( filename, world, self.world.isServer)

        self.staticMeshScene = StaticMeshScene( self.ents )
        self.dynamicEnts = [x for x in self.ents if not x.isStatic]
        #self.dynamicSphereChecker = mathhat.SphereVisChecker()


    def precacheGraphics(self):
        for ent in self.ents:
            ent.cacheGraphics()

    def getEntByName(self, name):
        for ent in self.ents:
            if ent.scriptname == name:
                return ent
        return None

    def getDynamicEntById(self, id):
        for ent in self.dynamicEnts:
            if ent.id == id:
                return ent
        return None

#Network Message Handlers
    def netSpawnEnt(self, msg):
        ent = EntLoader.MakeEntFromPacket(self.world, msg )
        self.ents.append(ent)
        self.dynamicEnts.append(ent)


    def netMoveActor(self, msg):
        act = self.getDynamicEntById(msg.entID)
        act.processMoveActorPacket( msg, .001 )

    def netMoveEnt(self, msg):
        ent = self.getDynamicEntById(msg.entID)
        ent.processMoveEntPacket( msg, .001 )


    def removeAllDynamicEnts(self):
        for x in self.dynamicEnts:
            if x is not self.world.playerEnt:
                x.removeFromWorld()
                self.ents.remove(x)
        del self.dynamicEnts
        self.dynamicEnts = []

    def update(self, time_step):
        """Update all the objects"""
        for e in self.ents:
            e.update( time_step )

        if self.world.graphicsEnabled:
            self._visibleDynamicEnts = self.dynamicObjectsVisibleInCamera(
                                            self.world.camera,
                                            excluded=[self.world.playerEnt] )
            DebugDisplay.update( "visible_dynamic_ents", len(self._visibleDynamicEnts ) )

    def dynamicObjectsVisibleInCamera( self, camera, excluded=[]):
        """Determines which objects are visible in a given camera"""
        #assert( self.world.playerEnt not in self.dynamicEnts )
        ents_to_test = self.dynamicEnts
        return  [ent for ent in ents_to_test
                    if ent.visibleIn(camera) and
                    ent not in excluded ]
        #return  [ent for ent in ents_to_test
        #            if ent not in excluded ]

    def draw(self):
        """Draw all the objects"""
        camera = self.world.camera
        self.staticMeshScene.draw(camera)
        for ent in self._visibleDynamicEnts:
            ent.draw()

    def drawAiInfo(self):
        for ent in self.dynamicEnts:
            if ent is not self.world.playerEnt:
                ent.drawAiInfo()


    def drawDebugInfo(self, font):
        for ent in self._visibleDynamicEnts:
            ent.drawDebugInfo(self.world.camera, font)


if __name__== '__main__':
    s = SceneManager( 'base', 'pymap' )
