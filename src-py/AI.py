import math
import Graphics
import MathUtil
import Platform
import Ents

from OpenGL.GL import glPushMatrix,glPopMatrix, glMultMatrixf
from cgkit.cgtypes import vec3
from mathhat import Frustrum
from Camera import AttachableCamera, MakeViewMatrix

class EntKnowledge(object):
	"""Stores known information about some object"""

	def __init__(self, ent):
		self.ent = ent
		self.world = ent.world
		self.info  = {}
		self.read( ent )

	def __getattr__(self, name):
		"""Allows direct indexing of knowledge base keys without
		directly referencing the info dictionary. So you can do this:

		some_info = kb.awesomeInfo

		instead of:

		some_info = kb.info['awesomeInfo'] """

		rval = None
		try:
			rval = self.info[name]
		except KeyError:
			raise AttributeError(name)
		return rval


	def read(self, ent ):
		self.addInfo( "location", ent.location )
		self.addInfo( "typeName", ent.typeName )
		self.update()

	def knownInfo(self):
		return self.info.keys()

	def addInfo( self, info_type_name, information ):
		self.info[info_type_name] = information
		self.update()

	def update(self):
		"""Updates the timestamp"""
		self.timestamp = Platform.time_in_seconds()


class AgentKnowledge(EntKnowledge):
	def read(self, agent ):
		self.addInfo( "location", agent.location )
		self.addInfo( "typeName", agent.typeName )
		self.addInfo( "isPlayer", self.world.isMainPlayer( agent ) )
		self.update()


class KnowledgeBase(object):
	def __init__(self, world, ent):
		self.world = world
		self.entity = ent
		self.ents = {} #key = ent, value= knowledge about object
		self._target = None
		self.pather = None

	#update functions
	def readSensorData( self, sensor_data ):
		for data in sensor_data:
			self.updateEntity( data.entity )

	def updateEntity( self, ent ):
		#read information out of entity and store information about it
		if ent in self.ents.keys():
			self.ents[ent].read( ent )
		else:
			self.ents[ent] = EntKnowledge(ent)

	def update(self):
		if self.pather:
			self.pather.step()

	def setTarget(self, target):
		assert isinstance( target, Ents.Ent ) or target == None
		self._target = target

	def getTarget(self):
		return self._target

	target = property(getTarget, setTarget)

	def isHoldingThingOfType(self, typename):
		if self.entity.isHoldingSomething() and self.entity.holding.typeName == typename:
			return True
		return False

	#query functions
	def infoForMainPlayer(self):
		if self.world.playerEnt in self.ents.keys():
			return self.ents[self.world.playerEnt]
		return None

	def infoForAllKnownOfType(self, type):
		return [v for v in self.ents.values() if v.typeName == type]

	def infoForCurrentTarget(self):
		return self.ents[self._target]

	def infoForAllKnownActors(self):
		pass

	def infoForAllKnownNonActors(Self):
		pass

	def infoAllEntities(self):
		pass

	def returnClosestThing(self, ent, things):
		if len(things) == 0: return None
		closest = things[0]
		for n in things:
			if (n.location-ent.location).length() < (closest.location-ent.location).length():
				closest = n
		return closest

	def drawDebugInfo(self):
		"""Draw debugging info"""
		if not self.pather: return
		self.pather.drawDebugInfo()


class Sensor(AttachableCamera):
	standardProjection = None
	def __init__(self, world, entity):
		AttachableCamera.__init__(self, entity )
		self.world  = world
		if not self.standardProjection:
			self.standardProjection = Graphics.Projection( float(4/3), 0.1, 300,90.0)
		self.projection = self.standardProjection

	def _calcViewMatrix(self):
		self.viewMatrix = MakeViewMatrix(self.ent.headLocation,
										self.ent.yaw,
										self.ent.pitch,
										self.ent.roll )

	def update(self, frametime):
		AttachableCamera.update(self, frametime)

	def snapshot(self):
		"""Generate a SensorData object for each visible object"""
		vis_func = self.world.scene.dynamicObjectsVisibleInCamera
		return [SensorData(x, (x.location - self.ent.location).length())
			       for x in vis_func(self, excluded=[self.ent])]

	def draw(self):
		#find camera's transform matrix
		m = MathUtil.buildTransformMatrix(self.ent.headLocation,
										  self.ent.yaw + math.pi,
										  self.ent.pitch,
										  self.ent.roll)
		glPushMatrix()
		glMultMatrixf( m.toList() )
		Graphics.drawPyramid(30, 10, 100.0)
		Graphics.drawWirePyramid(30, 10, 100.0)
		glPopMatrix()


class SensorData(object):
	def __init__(self, entity, distance):
		self.entity   = entity
		self.distance = distance

	def __str__(self):
		return "<Type: %s typeName: %s Distance: %s>" % (str(type(self.entity)), str(self.entity.typeName), str(self.distance) )



class PathFinder(object):
	def __init__(self, entity):
		self.entity = entity
		self.world = self.entity.world
		self.path = []
		self.pathGraphic = None
		self.nextPointIndex = 1
		self.targetPoint = None
		self.targetEnt = None

	def stillValid(self, epsilon=25):
		start = self.entity.location
		end = self.targetEnt.location
		if not self.world.terrain.isAreaPassable( end.x, end.z):
			return True
		if not self.world.terrain.isAreaPassable(start.x, start.z):
			return True
		if (self.targetEnt.location - self.targetPoint).length() > epsilon:
			return False
		return True

	def walking(self):
		return self.nextPointIndex < len(self.path)#(len(self.path) > 0)

	def reachedTarget(self):
		return (self.nextPointIndex >= len(self.path))
		#if not self.target: return False
		#return (self.distanceFromTarget2D() < 30 )

	def distanceFromTarget2D( self ):
		point   = vec3(self.targetPoint)
		return (point-self.entity.location).length()

	def setup(self, target_point, target_ent=None):
		self.nextPointIndex = 1
		self.targetPoint = vec3(target_point)
		self.targetEnt = target_ent
		start_time = Platform.time_in_seconds()
		self.path = self.entity.pathToLocation( self.targetPoint )
		end_time = Platform.time_in_seconds()
		self.pathGraphic = None
		#print "TIME: ", end_time-start_time
		#print self.path

	def step(self):
		if self.targetEnt:
			if not self.stillValid():
				self.setup( self.targetEnt.location, self.targetEnt )
		if self.nextPointIndex < len(self.path):
			#walk towards nextpoint
			path_point = self.path[self.nextPointIndex]
			target_vec = path_point-self.entity.location
			target_vec.y = 0
			self.entity.moveTowards(target_vec)
			self.entity.faceTowards(target_vec)
			if target_vec.length()<10:
				self.nextPointIndex+=1

	def drawDebugInfo(self):
		"""Draw debugging info"""
		#Graphics.drawSphere( self.entity.location, 40, 40, 40 )
		#Graphics.drawPath(self.path, 3, 10 )
		if not self.pathGraphic:
			self.pathGraphic = Graphics.Path( self.path, 3, 10 )
		self.pathGraphic.draw()
