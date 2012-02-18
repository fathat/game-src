from cgkit.cgtypes import *
from MatrixMath import *
from mathhat import Plane, Frustrum
import ListFunctions
import OpenGL.GL as GL
import Graphics
import MathUtil
from Graphics import GraphicsCard


def MakeViewMatrix( position, yaw, pitch, roll ):
    base   = mat4(1)
    mscale = mat4(1).scale(vec3(1,1,-1))
    mrot   = MakeRotationMatrix( -yaw, -pitch, -roll )
    mtrans = base.translation( position )
    #mtrans = MathUtil.buildTransformMatrix( position, -yaw, -pitch, -roll )
    #return mtrans.inverse()
    final  = mtrans * mrot * mscale
    return final.inverse()



class ICamera(object):
    def move( self, move_vector ):
        """Moves the camera by the move_vector. Note that the move_vector
        is defined in the camera's local space, so (0,0,-1) would move the
        camera forward regardless of where it's looking. Also note that
        if the camera is attached to something, it might also move that
        thing the camera is attached to."""

    def turn( self, yaw, pitch, roll ):
        """Turns the camera by a certain amount. If the camera is attached
        to something, it might also turn that thing. """

    def update(self, timestep):
        """Update the camera"""

    def getRotationMatrix( self):
        """Returns a 4x4 matrix for the rotation of the camera (but discludes
        translation)"""

    def getViewMatrix( self):
        """Returns a 4x4 matrix that OpenGL can use for setting the view."""

    def makeFrustrum(self):
        return Frustrum( self )

    def apply(self):
        pass



class Freecam (object):
    """Camera with free look"""

    def __init__(self):
        self.position = vec3( 0, 0, 0 )
        self.yaw   = 0
        self.pitch = 0
        self.roll  = 0
        self.frustrum = Frustrum()
        self.projection = GraphicsCard.screenProjection
        self.viewMatrix = None
        self.clipMatrix = None

    def apply(self):
        GraphicsCard.loadIdentity()
        GraphicsCard.multMatrix( self.getViewMatrix().toList() )

    def asFreecam(self):
        f = Freecam()
        f.position = vec3(self.position)
        f.yaw   = self.yaw
        f.pitch = self.pitch
        f.roll  = self.roll
        return f

    def move( self, move_vector ):
        r = self.getRotationMatrix()
        self.position = self.position + (r* move_vector )

    def turn( self, yaw, pitch, roll ):
        self.yaw += yaw
        self.pitch += pitch
        self.roll += roll

    def update(self, timestep):
        self._calcViewMatrix()
        self._calcClipMatrix()
        self.frustrum.update(self.getClipMatrix().toList() )

    def sphereVisible(self, sphere):
        c = sphere.center
        return self.frustrum.sphereInFrustrum( c.x, c.y, c.z, sphere.radius )

    def getRotationMatrix( self ):
        return MakeRotationMatrix( -self.yaw, -self.pitch, -self.roll )

    def getTransformMatrix(self):
        return MathUtil.buildTransformMatrix(self.position,
                                             self.yaw,
                                             self.pitch,
                                             self.roll)

    def getViewMatrix( self ):
        if self.viewMatrix is None: self._calcViewMatrix()
        return self.viewMatrix


    def getClipMatrix(self):
        if self.clipMatrix is None: self._calcClipMaptrix()
        return self.clipMatrix

    def _calcViewMatrix(self):
        #self.matrix = MathUtil.buildTransformMatrix(self.position,
        #                                           self.yaw,
        #                                           self.pitch,
        #                                           self.roll )
        self.viewMatrix = MakeViewMatrix( self.position, self.yaw, self.pitch, self.roll )

    def _calcClipMatrix(self):
        proj_m = self.projection.projectionMatrix
        modl_m = self.getViewMatrix()
        self.clipMatrix = (proj_m * modl_m)


class AttachableCamera( object ):
    """First person camera -- attach to an entity of some sort."""
    def __init__(self, ent, fov=60,aspect=float(4/3),  zoom = 0 ):
        self.ent  = ent
        self.zoom = zoom
        self.projection = GraphicsCard.screenProjection
        self.frustrum = Frustrum()

    def asFreecam(self):
        f = Freecam()
        f.position = vec3(self.position)
        f.yaw   = self.yaw
        f.pitch = self.pitch
        f.roll  = self.roll
        return f

    def apply(self):
        GraphicsCard.loadIdentity()
        GraphicsCard.multMatrix( self.getViewMatrix().toList() )

    def move(self, move_vector ):
        #r = self.getRotationMatrix()
        self.ent.moveForward( move_vector )

    def turn( self, yaw, pitch, roll ):
        self.ent.turn( yaw, pitch, roll )

    def update(self, timestep):
        self._calcViewMatrix()
        self._calcClipMatrix()
        #self.getClipMatrix().toList()
        self.frustrum.update(self.getClipMatrix().toList() )

    def sphereVisible(self, sphere):
        c = sphere.center
        return self.frustrum.sphereInFrustrum( c.x, c.y, c.z, sphere.radius )

    def getRotationMatrix( self ):
        return MakeRotationMatrix( -self.ent.yaw, -self.ent.pitch, -self.ent.roll )

    def _calcViewMatrix(self):
        e = self.ent
        self.viewMatrix = MakeViewMatrix( e.headLocation, e.yaw, e.pitch, e.roll )
        #self.matrix = MathUtil.buildTransformMatrix(self.ent.headLocation,
        #                                           self.ent.yaw,
        #                                           self.ent.pitch,
        #                                           self.ent.roll )
        #self.viewMatrix = self.matrix.inverse()

    def _calcClipMatrix(self):
        proj_m = self.projection.projectionMatrix
        modl_m = self.getViewMatrix()
        self.clipMatrix = (proj_m * modl_m)

    def getViewMatrix(self ):
        if not self.viewMatrix: self._calcViewMatrix()
        return self.viewMatrix

    def getClipMatrix(self):
        if not self.clipMatrix: self._calcClipMatrix()
        return self.clipMatrix

    def getposition( self ):
        return self.ent.headLocation

    def getyaw( self ):
        return self.ent.yaw

    def setyaw( self, yaw ):
        self.ent.yaw = yaw

    def getpitch( self ):
        return self.ent.pitch

    def setpitch( self, pitch ):
        self.ent.pitch = pitch

    def getroll( self ):
        return self.ent.roll

    def setroll( self, roll ):
        self.ent.roll = roll

    position = property( getposition )
    yaw      = property( getyaw, setyaw )
    pitch    = property( getpitch, setpitch )
    roll     = property( getroll, setroll )
