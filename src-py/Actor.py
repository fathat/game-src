import AI
import DebugDisplay
import MathUtil
import PlayerPhysics
import Settings

from cgkit.cgtypes import vec3, mat4
from Ents import Ent, DynamicEnt
from Graphics import GraphicsCard
from GameProtocol import GameProtocol

class Actor( DynamicEnt ):
    def __init__(self, world, ent_xml_data, ent_config ):
        DynamicEnt.__init__( self, world, ent_xml_data, ent_config )
        self._readLocationFromPhysics()
        self.yaw = self.initialData.yaw
        self.pitch = self.initialData.pitch
        self.roll = self.initialData.roll
        self.meshOffset = vec3( 0, -6, 0 )
        self.moveVector = vec3(0,0,0)
        if ent_config.speed:
            self.maxSpeed = ent_config.speed
        else:
            self.maxSpeed = 40
        self.maxForce = 200
        self.yawforce   = 0
        self.pitchforce = 0
        self.rollforce  = 0
        self.health = 100
        self.onGround = False
        self.groundNormal = vec3()
        self.sensor = AI.Sensor(self.world, self) #normal actors don't have sensors
        self.sensorData = []
        self.knowledge = AI.KnowledgeBase(world,self)
        self.holding = None

    @property
    def massConstant(self): return 1

    def damage(self, amount):
        self.health -= amount
        print "Damaged", amount

    def reset(self):
        self._resetLocation()
        self.knowledge = AI.KnowledgeBase(self.world,self)
        self.health = 100
        self.drop()

    @property
    def body(self): return self.construction.body

    def _createPhysicsBody(self):
        #self.construction = PlayerPhysics.RollingAvatar(self.world, self, self.massConstant, 5)
        self.construction = PlayerPhysics.SlidingAvatar(self.world,
                                                        self,
                                                        self.massConstant,
                                                        2.5,
                                                        7)
        self.construction.setTransformCallback(self.matrixChanged)

    def matrixChanged(self, body, matrix):
        self._matrix = mat4( matrix ).transpose()
        self._readLocationFromPhysics()
        self._adjustBoundingVolume()

    def removeFromWorld(self):
        self.construction.removeFromWorld()
        del self.construction

    def setLocationAndRotation(self, location, *args ):
        self.location = location
        if len(args) == 3:
            self.yaw, self.pitch, self.roll = args
        else:
            self.yaw, self.pitch, self.roll = MathUtil.quaternionToEulerYawPitchRoll(*args)
        self.construction.setLocation(location, self.yaw, self.pitch, self.roll)
        self._matrix = MathUtil.buildTransformMatrix(self.location, self.yaw, self.pitch, self.roll)

    @property
    def headLocation(self):
        return self.location + vec3(0,5.9,0)
    #headLocation = property(getheadlocation)

    def isHoldingSomething(self):
        return (self.holding != None)

    def getVelocity(self):
        return vec3(self.body.GetVelocity() )

    def throwAt( self, target):
        if not self.isHoldingSomething(): return
        print "Throwing at:", target
        thing = self.drop()
        #apply a force
        #self.holding.body.SetMassMatrix( 8, 1.0, 1.0, 1.0 )
        thing.body.SetVelocity((0,0,0))
        box_loc = thing.location
        velocity = vec3(target.body.GetVelocity())
        future_location = target.headLocation + (velocity/2.0)
        target_vec = ( future_location-thing.location).normalize() * 200.0
        thing.body.AddBodyImpulse((target_vec.x, target_vec.y, target_vec.z ),
                                  (thing.location.x, thing.location.y, thing.location.z) )
        thing.owner = None

    def drop(self):
        """Drops whatever it's holding"""
        if not self.isHoldingSomething(): return
        thing = self.holding
        thing.owner = None

        #release the hold on the box
        self.slider.ForceDestroy()
        del self.slider
        self.slider = None
        self.holding = None
        return thing

    def grab(self, thing):
        self.holding = thing
        thing.owner = self
        location = self.location + vec3(0,9,0)
        m = MathUtil.buildTransformMatrix( location, 0, 0, 0)
        thing.body.SetMatrix( m.toList() )
        thing.body.SetVelocity( self.body.GetVelocity() )
        self.slider = self.world.solver.makeSliderJoint( (location.x, location.y, location.z), (0,1,0), self.body, thing.body )
        self.slider.SetMinLimit(7.5)
        self.slider.SetMaxLimit(10)

    def faceTowards(self, direction_vector):
        if direction_vector.length():
            #figure out the current direction
            current_direction = MathUtil.rotateVectorAroundAxis( vec3(0,0,1), self.yaw, vec3(0,1,0) )
            direction_vector = direction_vector.normalize()
            # interpolate between current direction and desired direction
            d = MathUtil.interpolateVectors( current_direction, direction_vector, 0.1 )
            self.yaw = MathUtil.yawAngle( d )

    def makeUpdatePacket(self):
        return str(GameProtocol.MoveActor(self.entID,
                                          self.location.x,
                                          self.location.y,
                                          self.location.z,
                                          self.yaw,
                                          self.pitch))

    def moveTowards(self, direction):
        """Works like move, but entity moves at a self imposed speed limit"""
        if direction.length() < 0.00001:
            self.move( vec3() )
            return
        self.move( (direction.normalize()) * self.maxSpeed )

    def moveTo( self, point):
        target_vec = point - self.location
        self.faceTowards( target_vec)
        self.moveTowards( target_vec )

    def processMoveActorPacket(self, packet, ping):
        self.setLocationAndRotation(vec3(packet.x, packet.y, packet.z),
                                    packet.yaw, packet.pitch, 0)
        self._readLocationFromPhysics()
        self._adjustBoundingVolume()

    def processMoveEntPacket(self, packet, ping ):
        DynamicEnt.processMoveEntPacket(self, packet, ping )
        self.moveTowards( vec3(packet.vx, packet.vy, packet.vz))

    def move(self, move_vector ):
        move_vector_2d = vec3(move_vector.x, 0, move_vector.z)
        if move_vector_2d.length() > self.maxSpeed:
            move_vector_2d = move_vector_2d.normalize() * self.maxSpeed
            move_vector.x, move_vector.z = move_vector_2d.x, move_vector_2d.z
        self.construction.setDesiredVelocity( move_vector)

    def moveForward(self, move_vector):
        move_vector_2d = vec3(move_vector.x, 0, move_vector.z)
        if move_vector_2d.length() > self.maxSpeed:
            move_vector_2d = move_vector_2d.normalize() * self.maxSpeed
            move_vector.x, move_vector.z = move_vector_2d.x, move_vector_2d.z
        self.construction.setDesiredVelocity( move_vector, -self.yaw)

    def turn( self, yaw, pitch, roll ):
        self.yaw   += yaw
        self.pitch += pitch
        self.roll  += roll

    def pathToTarget(self, ent):
        return self.world.path( self.location, ent.location)

    def pathToLocation(self, location):
        return self.world.path( self.location, location)

    def setGraphicsTransform( self ):
        GraphicsCard.multMatrix( MathUtil.buildTransformMatrix(self.location, self.yaw, 0, 0 ).toList() )

    def _checkIfOnGround(self):
        """Check if the player is on the ground"""
        self.onGround = False
        def rayHit( body, normal, collision_id, userdata, intersectParam):
            DebugDisplay.update( "collision_id", collision_id)
            self.onGround = True
            self.groundNormal = vec3(normal)
            return intersectParam
        self.world.solver.rayCast(self.location, self.location-vec3(0,8.5,0), rayHit, None)

    def setTransform( self ):
        GraphicsCard.multMatrix( self.getTransformMatrix().toList() )

    def _updateAI(self, timedelta): pass

    def _updatePhysics(self, timedelta):
        max_force = self.maxForce
        if self.location.y > self.world.seaLevel:
            if self.onGround and self.groundNormal.y < 0.85 and self.groundNormal.length():
                v = vec3(self.groundNormal)
                v.y = 0
                if v.length() > 0:
                    v = v.normalize()
                self.construction.addToVelocity( v*100)
            elif not self.onGround:
                max_force /= 10
        self.construction.updateForces(timedelta, max_force)

    def update( self, timedelta ):
        #Check if the player is touching the ground
        #This is useful for determining if the player
        #should be allowed to jump.
        self._checkIfOnGround()
        self._updatePhysics(timedelta)
        self._updateAI(timedelta)
        #self._determineGraphicsMatrix()

class AiActor(Actor):
    def __init__( self, world, ent_xml_data, ent_config ):
        Actor.__init__( self, world, ent_xml_data, ent_config )
        self.sensor = AI.Sensor(world, self)
        self.maxForce = 1000
        self.message_queue = []

    @property
    def massConstant(self): return 1

    def _checkForDeath(self):
        if self.headLocation.y < self.world.seaLevel:
            self.health = 0
        if self.health <= 0:
            print "Reseting", self
            if self.typeName == 'ninja':
                self.world.ninjasKilled += 1
            if self.typeName == 'pirate':
                self.world.piratesKilled += 1
            self.world.makeDeathBlotch( vec3( self.location ) )
            self.reset()

    def _updateAI(self, timedelta):
        if not Settings.RunAI: return
        self.construction.clearVelocity()
        self._checkForDeath()
        self.sensor.update(timedelta)
        self.sensorData = self.sensor.snapshot()
        #self.sensorData = []
        self.knowledge.readSensorData( self.sensorData )
        self.knowledge.update()
        #update behaviors here
        for behavior in self.behaviors:
            behavior.update( self.knowledge, self.message_queue, timedelta )
        self.message_queue = []

    def drawAiInfo(self):
        if Settings.DrawViewFrustrums:
            self.sensor.draw()
        if Settings.DrawPaths:
            self.knowledge.drawDebugInfo()
        for b in self.behaviors:
            b.drawDebugInfo()

    def drawDebugInfo(self, camera, font):
        """Draws text above the object with debug information"""
        pass
        #sx,sy,sz = GraphicsCard.projectWorldPointToScreen(
        #                    self.location.x, self.location.y, self.location.z,
        #                    camera.viewMatrix )
        ##lines = ( ["Type: " + self.typeName]
        ##         + [str(type(x)) for x in self.behaviors]
        ##         + ["Looking At: " ]
        ##         + [str(x) for x in self.sensorData] )
        ##debug_string = "\n".join( lines )
        ##font.draw( sx, sy, debug_string )
        #font.draw( sx, sy, "L:" + str( self.location )   )
