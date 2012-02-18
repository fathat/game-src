#!/usr/bin/env python
from cgkit.cgtypes import vec3, mat4
import math
import Physics
import MathUtil
import Settings

"""def HaltTorque( body):
    mass, ix, iy, iz = body.GetMassMatrix()
    cameraDir = vec3( 0, 0, 1 )
    rotationMatrix = MatrixMath.MakeRotationMatrix( -self.yaw, -self.pitch, -self.roll )
    cameraDir = rotationMatrix * cameraDir
    frontVec = MatrixMath.MatrixFrontVector( rotationMatrix )
    cameraVec = vec4(cameraDir.x, cameraDir.y, cameraDir.z, 1)
    vt = vec3(frontVec.x*cameraDir.x,frontVec.y*cameraDir.y,frontVec.z*cameraDir.z)
    steerAngle = min( max( vt.y, -1.0), 1.0 )
    steerAngle = math.asin( steerAngle)
    DebugDisplay.update( "steerAngle", steerAngle )
    DebugDisplay.update( "cameraVec", cameraVec )
    DebugDisplay.update( "frontVec", frontVec )
    omegaX, omegaY, omegaZ = body.GetOmega()

    torque = vec3( 0.0, 0.5*iy*((0.99)*steerAngle-omegaY)*0.99, 0.0)
    body.SetTorque( (torque.x, torque.y, torque.z) )

    #body.AddTorque( (self.yawforce, self.pitchforce, self.rollforce) )
    body.SetAngularDamping((0.1,1.0,0.1))
    body.SetLinearDamping( 0.1 )"""



class SlidingAvatar(object):
    def __init__(self, world, ent, mass, radius, height):
        self.world  = world
        self.mass   = mass
        self.radius  = radius
        self.height = height
        self.moveVector = vec3()
        self.moveForce = vec3()
        self.resetVelocityEveryFrame = False

        #Construct the sphere and body
        sphere = self.world.solver.makeSphere(radius, height, radius, None )
        self.body = self.world.solver.makeBody( sphere, ent )
        self.body.SetMaterialGroupID( self.world.materialForType('actor') )
        del sphere

        self.body.SetMassMatrix( self.mass, radius, height, radius)
        self.joint = self.world.solver.makeUpVectorJoint( self.body, (0,1,0) )
        self.body.ApplyForceAndTorqueCallback = self._applyForces
        self.body.player = True
        self.body.entRef = ent

    def setTransformCallback(self, callback):
        self.body.TransformCallback = callback

    def removeFromWorld(self):
        self.world.solver.removeBody(self.body)
        del self.body

    def setLocation(self, location, yaw, pitch, roll):
        self.location = location
        self.matrix = MathUtil.buildTransformMatrix(self.location, yaw, pitch, roll)
        self.body.SetMatrix( self.matrix.toList() )

    def clearVelocity(self):
        self.moveVector = vec3(0,0,0)

    def setDesiredVelocity(self, vector, yaw=0):
        """Sets the motion vector for the avatar to move in."""
        self.body.Unfreeze()
        if yaw:
            self.moveVector = MathUtil.rotateVectorAroundAxis(vector,
                                                              yaw,
                                                              vec3(0,1,0))
        else:
            self.moveVector = vector

    def addToVelocity(self, vector):
        self.moveVector = self.moveVector + vector

    def _applyForces( self, body ):
        """Called by the physics engine during its update loop"""
        #DebugDisplay.update( "move_force", move_force )
        mass, ix, iy, iz = body.GetMassMatrix()
        gravity_vector = ( 0, -mass*Physics.Gravity, 0 )
        body.SetForce( gravity_vector  )
        body.AddBuoyancyForce( 1/100.0, 0.8, 0.8,
                          gravity_vector,
                          (0,1,0,-15))
        body.AddForce( (self.moveForce.x, self.moveForce.y, self.moveForce.z) )
        if self.resetVelocityEveryFrame:
            self.moveVector = vec3(0,0,0)

    def updateForces(self, timestep, maxForce, yaw=0):
        """ Determines how much force is needed to move the player at the
        desired speed. If yaw is specified, motion is relative to that angle."""
        if not Settings.ApplyPlayerForces:
            self.moveForce = vec3()
            return
        self.moveForce = Physics.DetermineMoveForceForVelocity (self.moveVector,
                                                                maxForce,
                                                                self.body,
                                                                timestep,
                                                                yaw)



class RollingAvatar(object):
    """A body that represents a character"""

    def __init__(self, world, ent, mass, radius):

        #store thingies
        self.world  = world
        self.mass   = mass
        self.radius = radius
        self.moveVector = vec3()
        self.moveForce = vec3()

        #create the collision sphere and body
        sphere = self.world.solver.makeSphere( radius, radius, radius, None )
        self.body = self.world.solver.makeBody( sphere, ent )
        self.body.SetMaterialGroupID( self.world.materialForType('actor' ) )
        self.body.SetAngularDamping((0.9, 0.9, 0.9) )
        self.body.SetLinearDamping( 0.09 )
        del sphere
        self.joint = self.world.solver.makeUpVectorJoint( self.body, (0,1,0) )

        self._setBodyMass()

        #create the joint to force the body to face up
        self.body.ApplyForceAndTorqueCallback = self._applyForces

    def _setBodyMass(self):
        ix = self.mass * self.radius
        self.body.SetMassMatrix( self.mass, ix, ix, ix )

    def setLocation(self, location):
        self.location = location
        self.matrix = MathUtil.buildTransformMatrix(self.location, 0, 0, 0)
        self.body.SetMatrix( self.matrix.toList() )

    def setDesiredVelocity(self, vector):
        self.moveVector = vec3(vector)
        if self.moveVector.length() and self.joint:
            self.joint.ForceDestroy()
            self.joint = None
        elif not self.moveVector.length() and not self.joint:
            self.joint = self.world.solver.makeUpVectorJoint( self.body, (0,1,0) )

    def updateForces(self, timedelta, maxForce, yaw=0):
        #take moveVector, rotate it by yaw, and scale it
        strafe_force = -self.moveVector.x
        fwd_force = self.moveVector.z
        torque_vec = vec3( fwd_force, 0, strafe_force )
        self.moveForce = MathUtil.rotateVectorAroundAxis(torque_vec, yaw, vec3(0,1,0), True )
        self.moveForce = self.moveForce * 20

    def _applyForces( self, body ):
        """Called by the physics engine during its update loop"""
        #DebugDisplay.update( "move_force", move_force )
        mass, ix, iy, iz = body.GetMassMatrix()
        gravity_vector = ( 0, -mass*Physics.Gravity, 0 )
        body.SetForce( gravity_vector  )
        body.AddBuoyancyForce( 1/100.0, 0.8, 0.8,
                          gravity_vector,
                          (0,1,0,-15))
        body.AddTorque( ( self.moveForce.x, self.moveForce.y, self.moveForce.z ) )
        #self.moveVector = vec3(0,0,0)
