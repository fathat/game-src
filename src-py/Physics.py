import newton
import MathUtil
from cgkit.cgtypes import vec3

### Constants #################################################################
Gravity = 9.8*5

### Utility functions #########################################################

def DetermineMoveForceForVelocity( velocity, maxForce, body, timedelta, yaw=0):
    """Returns the amount of force to apply to the body to move it at the desired velocity"""
    assert timedelta > 0
    mass, ix, iy, iz = body.GetMassMatrix()
    fx, fy, fz = velocity.x, velocity.y, velocity.z
    vx, vy, vz = body.GetVelocity()

    #figure out neccessary force to move player at desired velocity
    move_force = MathUtil.playerMoveForce(vec3(vx, 0, vz), -yaw, fz, fx,
                                          mass, timedelta, maxForce)
    move_force.y = fy
    return move_force

def MoveForceForNetworkObject(body, srv_position, srv_velocity, ping):
    #predict where the packet is going to be
    future_position = srv_position + srv_velocity*ping

    #figure out the velocity the body needs to be moving at to get to
    #future_position within the ping time






### Classes ###################################################################

class PhysicsBody(newton.Body):
    def __init__(self, world, cg):
        self.touchingGround = False #"Ground" basically being any object under this object's "feet"
        self.movable   = False
        newton.Body.__init__(self, world, cg)

    def SetMassMatrix( self, mass, ix, iy, iz ):
        if mass > 0:
            self.movable = True
        else:
            self.movable = False
        return newton.Body.SetMassMatrix( self, mass, ix, iy, iz )

class Solver(object):
    """Interface to the physics library"""

    def __init__(self):
        """Initialize the solver"""
        self.world = newton.World()
        """self.world.MaterialSetCollisionCallback(
                    0,
                    0,
                    self.world,
                    DefaultBeginCollisionCallback,
                    DefaultProcessCollisionCallback,
                    DefaultEndCollisionCallback )"""
        print "Using Newton Version: " + str( self.world.GetVersion() )


    #Factory functions for making various collision geometries
    def makeBox( self, width, height, depth, offset_matrix=None ):
        return newton.Box( self.world, width, height, depth, offset_matrix )

    def makeSphere( self, rx, ry, rz, offset_matrix=None ):
        return newton.Sphere( self.world, rx, ry, rz, offset_matrix )

    def makeMesh( self ):
        return newton.TreeCollision( self.world )

    def makeConvexHull( self, list, offset_matrix = None ):
        return newton.ConvexHull( self.world, list, offset_matrix )

    def makeCylinder( self, radius, height, offset_matrix = None ):
        return newton.Cylinder( self.world, radius, height, offset_matrix )

    def makeCone( self, radius, height, offset_matrix=None ):
        return newton.Cone( self.world, radius, height, offset_matrix )

    def makeChamferCylinder( self, radius, height, offset_matrix=None ):
        return newton.ChamferCylinder( self.world, radius, height, offset_matrix )

    def makeNullCollider(self):
        return newton.NullCollider(self.world)

    def makeBody( self, collision_geom, owner ):
        body = PhysicsBody( self.world, collision_geom )
        body.owner = owner
        return body

    def makeHeightmap ( self, size, cell_size, height_values ):
        return newton.HeightField( self.world, size, cell_size, height_values )

    #material stuff
    def makeMaterialGroupId( self ):
        return self.world.MaterialCreateGroupID()

    def defaultGroupID(self):
        return self.world.MaterialGetDefaultGroupID()

    #Joints
    def makeUpVectorJoint( self, body, pindir ):
        return newton.UpVector( self.world, pindir, body )

    def makeSliderJoint( self, location, direction, parent, child ):
        return newton.Slider( self.world, location, direction,child, parent )

    #world stuff
    def getWorldSize( self ):
        return self.world.GetWorldSize()

    def setWorldSize( self, min, max ):
        self.world.SetWorldSize( min, max )

    def getTimeStep(self):
        return self.world.GetTimeStep()

    def setMinimumFramerate( self, min_framerate ):
        self.world.SetMinimumFrameRate( min_framerate)

    def update( self, time_step ):
        self.world.Update( time_step )

    def rayCast(self, p0, p1, callback, userdata):
        a = (p0.x, p0.y, p0.z)
        b = (p1.x, p1.y, p1.z)
        return self.world.RayCast( a,b, callback, userdata)

    def removeBody(self, body):
        self.world.RemoveBody(body)

    #material stuff
    def setDefaultElasticity(self, id1, id2, elasticity):
        self.world.MaterialSetDefaultElasticity( id1, id2, elasticity)

    def setDefaultSoftness(self, id1, id2, softness):
        self.world.MaterialSetDefaultSoftness( id1, id2, softness)

    def setDefaultFriction(self, id1, id2, static, kinetic):
        self.world.MaterialSetDefaultFriction( id1, id2, static, kinetic)


### Default Callbacks #########################################################

#Callback for material. Doesn't actually do anything.
def DefaultBeginCollisionCallback( material, body1, body2, userdata ):
    pass

#Callback for material. Doesn't actually do anything.
def DefaultProcessCollisionCallback ( material, contact, userdata ):
    #print material
    pass

def DefaultEndCollisionCallback( material, userdata ):
    #print "end"
    pass

def ApplyStandardGravityForce(body):
    mass, ix, iy, iz = body.GetMassMatrix()
    gravity_vector = ( 0, -mass*Gravity, 0 )
    body.AddForce( gravity_vector  )

def StandardFallCallback(body):
    mass, ix, iy, iz = body.GetMassMatrix()
    gravity_vector = ( 0, -mass*Gravity, 0 )
    body.SetForce( gravity_vector  )
    body.AddBuoyancyForce( 1/100.0, 0.8, 0.8,
                          gravity_vector,
                          (0,1,0,-15))
