from LangExt import *
import AI
import Behaviors
import ConfigParser
import Graphics
import MathUtil
import MatrixMath
import MeshLoader
import string
import math
import TextUtil
import Platform
import DebugDisplay
import Physics
import os.path
import Settings
import PlayerPhysics
from Graphics import GraphicsCard
from cgkit.cgtypes import *
from GameProtocol import GameProtocol

class Ent( object ):
    """Base class for entities."""
    serverside = False

    def __init__(self, world, ent_xml_data, ent_config):
        """Initializes location data and mesh filename """
        self.initialData  = ent_xml_data
        self.typeInfo = ent_config
        self.world = world
        self.scriptname = ent_xml_data.scriptname
        self.meshOffset = vec3( 0, 0, 0)
        self.meshFile = Platform.asOSPath( ent_config.meshFile )
        self.behaviors = [x(self) for x in ent_config.behaviors]
        self.typeName = ent_config.typeName
        self.boundingSphere = None
        self.owner = None
        self.netVector = None

        if world.isServer and self.serverside:
            self.entID = world.generateEntID()
        else:
            self.entID = None
        self._createPhysicsBody()
        self._resetLocation()

    #Private Functions
    def _adjustBoundingVolume(self):
        if self.mesh:
            self.boundingSphere = self.mesh.boundingSphereAt( self.matrix )
        else:
            self.boundingSphere = None

    def _createPhysicsBody(self):
        pass

    def _resetLocation(self):
        self.setLocationAndRotation(
                         vec3( self.initialData.x, self.initialData.y, self.initialData.z ),
                         self.initialData.yaw,
                         self.initialData.pitch,
                         self.initialData.roll)

    #Properties
    @property
    def active(self):
        return (self.body.GetSleepingState() == 1)

    @property
    def isStatic(self): return False

    @property
    def matrix(self): return self._matrix

    @property
    def mesh(self):
        return MeshLoader.LoadMeshFromFile( self.meshFile )

    #Public functions
    def cacheGraphics(self):
        """Loads any graphical resources the entity uses into memory"""
        self.mesh.precacheDeviceResources()

    def distanceFrom(self, location):
        return (self.location - location).length()

    def draw( self ):
        if not self.meshFile: return
        mesh = MeshLoader.LoadMeshFromFile( self.meshFile )
        GraphicsCard.pushMatrix()
        self.setGraphicsTransform()
        GraphicsCard.translate( self.meshOffset.x, self.meshOffset.y, self.meshOffset.z)
        mesh.draw()
        GraphicsCard.popMatrix()

    def drawAiInfo(self):
        """Draws AI debug info"""

    def drawDebugInfo(self, camera, font):
        """Draws text above the object with debug information"""

    def inImpassableArea( self ):
        return not self.world.terrain.isAreaPassable( self.location.x, self.location.z )

    def matrixChanged( self, body, matrix ):
        self._matrix = mat4( matrix ).transpose()
        self._adjustBoundingVolume()

    def moveForward( self, move_vector): pass

    def move (self, move_vector ): pass

    def removeFromWorld(self):
        """Removes the entity from the world"""

    def reset(self):
        self._resetLocation()

    def setGraphicsTransform( self ):
        GraphicsCard.multMatrix( self.body.GetMatrix() )

    def setLocationAndRotation(self, location, *args ):
        if len(args) == 3:
            self._matrix = MathUtil.buildTransformMatrix(location, *args)
        else:
            self._matrix = MathUtil.buildTransformMatrixQ(location, *args)
        if hasattr(self, "body") and self.body is not None:
            self.body.SetMatrix( self._matrix.toList() )

    def turn( self, yaw, pitch, roll ): pass

    def update( self, frametime ):
        pass

    def visibleIn(self, camera):
        if self.boundingSphere:
            return camera.sphereVisible(self.boundingSphere )
        return False


class DynamicEnt( Ent):
    """An entity that is capable of moving."""
    serverside = True

    def _readLocationFromPhysics(self):
        m  = mat4( self.body.GetMatrix() )
        v4 = m.getRow(3)
        self.location = vec3( v4.x, v4.y, v4.z )
        self.q4 = quat().fromMat(m.getMat3().transpose())

    @make_property
    def velocity():
        doc = "The ent's current overall velocity"
        def fget(self):
            return vec3(*self.body.GetVelocity())
        def fset(self, value):
            if isinstance(value, vec3):
                self.body.SetVelocity( (value.x, value.y, value.z) )
            else:
                self.body.SetVelocity( value )
            self.body.Unfreeze()
        def fdel(self):
            pass #nothing to delete, really
        return locals()

    def makeUpdatePacket(self):
        return str(GameProtocol.MoveEnt(self.entID,
                             self.location.x, self.location.y, self.location.z,
                             self.q4.x, self.q4.y, self.q4.z, self.q4.w, 0, 0, 0))


    def processMoveEntPacket( self, packet, ping ):
        self.setLocationAndRotation(vec3(packet.x, packet.y, packet.z),
                                    quat(packet.qw, packet.qx, packet.qy, packet.qz))
        self.netVector = packet.vx, packet.vy, packet.vz

    def removeFromWorld(self):
        self.world.solver.removeBody( self.body )
        del self.body

    def setGraphicsTransform( self ):
        if Settings.DebugTransforms:
            m = MathUtil.buildTransformMatrixQ( self.location, self.q4)
            GraphicsCard.multMatrix( m.toList() )
        else:
            GraphicsCard.multMatrix( self.body.GetMatrix() )

    def update(self, timedelta):
        self._readLocationFromPhysics()
        self._adjustBoundingVolume()


class StaticObject( Ent ):
    """An entity that never moves. Represented in the physics system as a
    mesh with infinite mass"""
    def __init__( self, world, ent_xml_data, ent_config ):
        #call superclass constructor
        Ent.__init__(self, world, ent_xml_data, ent_config )
        self.location = vec3(self.initialData.x, self.initialData.y, self.initialData.z)
        #get the bounding sphere
        if self.mesh:
            self.boundingSphere = self.mesh.boundingSphereAt( self.matrix)
        else:
            self.boundingSphere = None

    def _createPhysicsBody( self ):
        mesh = MeshLoader.LoadMeshFromFile( self.meshFile )
        if mesh != None:
            phys_mesh = mesh.getPhysicsMesh(self.world.solver )
            self.body = self.world.solver.makeBody( phys_mesh, self )
            self.body.SetMaterialGroupID( self.world.materialForType('ground' ) )
            self.body.TransformCallback = self.matrixChanged
        else:
            self.body = None

    @property
    def isStatic(self): return True

    def visibleIn(self, camera):
        if self.boundingSphere:
            return camera.sphereVisible( self.boundingSphere )
        return False

    def update(self, frametime):
        pass #No updates neccessary since object never moves

class Vegetation( StaticObject):
    def _createPhysicsBody(self): pass


class Crate( DynamicEnt ):
    def __init__( self, world, ent_xml_data, ent_config ):
        #call superclass's constructor
        Ent.__init__(self, world, ent_xml_data, ent_config )

    def _createPhysicsBody(self):
        #Parse information about box size from the config file
        w = float( self.typeInfo.config.get( "box", "width" ) )
        h = float( self.typeInfo.config.get( "box", "height" ) )
        d = float( self.typeInfo.config.get( "box", "length" ) )

        if self.world.isServer:
            collider = self.world.solver.makeBox( w, h, d )
        else:
            collider = self.world.solver.makeNullCollider()
        self.body = self.world.solver.makeBody(collider, self )
        self.body.SetMaterialGroupID( self.world.materialForType('crate' ) )
        self.body.SetMassMatrix(2.0, w, h, d)

        #self._resetLocation()

        def fallDown( body ):
            mass, ix, iy, iz = body.GetMassMatrix()
            body.AddForce( ( 0, -mass*Physics.Gravity, 0 ) )

        if self.world.isServer:
            self.body.ApplyForceAndTorqueCallback = Physics.StandardFallCallback#fallDown
        self.body.TransformCallback = self.matrixChanged
        del collider
        self._readLocationFromPhysics()

    def reset(self):
        location = vec3(self.initialData.x, self.initialData.y, self.initialData.z )
        self.setLocationAndRotation(location,
                                    self.initialData.yaw,
                                    self.initialData.pitch,
                                    self.initialData.roll)
        self.health = 100

    def moving(self):
        return self.velocity.length() > 0.5

    def setTransform( self ):
        GraphicsCard.multMatrix( self.body.GetMatrix() )
