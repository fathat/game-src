from Camera import AttachableCamera, MakeViewMatrix
import Graphics
from mathhat import Frustrum
import math
import MathUtil
import Platform
import Ents
from AI import PathFinder
from OpenGL.GL import glPushMatrix, glPopMatrix, glMultMatrixf
from cgkit.cgtypes import vec3

class Message(object):
	def __init__(self, type, data):
		self.type = type
		self.data = data
		self.handled = False
		self.timestamp = Platform.time_in_seconds()



class Behavior(object):
	def __init__(self, ent):
		self.entity = ent

	def update(self, kb, message_queue, timedelta):
		pass

	def drawDebugInfo(self):
		pass


class ScanForThrowables(Behavior):
	def __init__(self, ent):
		Behavior.__init__(self, ent)

	def update(self, kb, message_queue, timedelta):
		#if kb.target and kb.target.owner != None:
		#	kb.target = None

		#dont try to pickup anything if entity is already holding an object
		if self.entity.isHoldingSomething():
			return

		#find something if entity doesn't have a target
		if not kb.target:
			throwables = [x for x in kb.infoForAllKnownOfType("crate")
						  #if not x.ent.thrown ]
						  if (not x.ent.moving() and not x.ent.inImpassableArea()
						  and x.ent.owner == None) ]

			if len(throwables) == 0: return
			target_info = throwables[0]
			for n in throwables:
				l1 = (n.ent.location - self.entity.location).length()
				l2 = (target_info.ent.location-self.entity.location).length()
				if l1 < l2:
					target_info = n
			kb.target = target_info.ent
			kb.target.owner = self.entity

			#repath to get to the throwable
			kb.pather = PathFinder(self.entity)
			kb.pather.setup(vec3(target_info.location), target_info.ent )
			print "Found thing to throw! (", target_info.typeName, ")"

class PickupNearbyItem(Behavior):
	def __init__(self, ent ):
		Behavior.__init__(self, ent)
		self.waitTime = 0


	def update(self, kb, message_queue, timedelta):
		if self.entity.isHoldingSomething(): return
		self.waitTime = max(self.waitTime-timedelta, 0)
		target = kb.target
		if target == None:
			return
		if target.typeName != 'crate':
			return
		distance_vector = target.location - self.entity.location
		l = distance_vector.length()

		if l > 20 and l < 50:
			kb.pather = None
			self.entity.moveTowards( distance_vector )
		elif l <= 20 and self.waitTime <= 0.0001:
			print "Grabbing...", target
			self.entity.grab( target )
			kb.target = None
			kb.pather = None
			self.waitTime = 2



class FollowMainPlayer(Behavior):
	"""A test behavior"""
	def __init__(self, ent):
		Behavior.__init__(self, ent)

	def update(self, kb, message_queue, timedelta):
		#search sensordata for main player
		main_player_info = kb.infoForMainPlayer()
		if not main_player_info: return

		#main player visible, so follow him
		target_vec = main_player_info.location - self.entity.location
		if target_vec.length() > 50:
			self.entity.moveTowards( target_vec )
			self.entity.faceTowards( target_vec )
		else:
			self.entity.faceTowards( target_vec )

class WalkToCenter(Behavior):
	"""Another test behavior"""
	def __init__(self, ent):
		Behavior.__init__(self, ent)
		self.goToCenter = True

	def update( self, kb, message_queue, timedelta):
		if kb.target: return
		world = kb.world

		treasures_known = [x for x in kb.infoForAllKnownOfType("treasurechest") if x.ent.owner is None ]

		map_center = world.getEntByName("MapCenter")
		distance_to_center = (map_center.location - self.entity.location).length()

		#if not pathing anywhere...
		if not kb.pather and distance_to_center >= 300:
			#then walk to center of the map if we don't know where the treasure is...
			if len(treasures_known) == 0:
				kb.pather = PathFinder(self.entity)
				kb.pather.setup(vec3(map_center.location))
				self.goToCenter = True
			#otherwise, walk to the treasure
			if len(treasures_known) > 0:
				treasure = kb.returnClosestThing(self.entity, treasures_known)
				kb.pather = PathFinder(self.entity)
				kb.pather.setup(treasure.location)
				self.goToCenter = True

		if kb.pather and distance_to_center <= 20 and self.goToCenter:
			kb.pather = None
			self.goToCenter = False


class BasicWalkToCenter(Behavior):
	"""Another test behavior"""
	def __init__(self, ent):
		Behavior.__init__(self, ent)

	def update( self, kb, message_queue, timedelta):
		world = kb.world

		#walk to center of the map if we don't know where the treasure is...
		if not kb.pather:
			kb.pather = PathFinder(self.entity)
			kb.pather.setup(vec3(world.getEntByName("MapCenter").location))


class WalkToPirateFlag(Behavior):
	"""Another test behavior"""
	def __init__(self, ent):
		Behavior.__init__(self, ent)

	def update( self, kb, message_queue, timedelta):
		world = kb.world

		#if not self.pathto.walking():
		if kb.isHoldingThingOfType( "treasurechest"):
			flag = world.getEntByName("PirateFlag")
			if not kb.pather:
				kb.pather = PathFinder(self.entity)
				kb.pather.setup(vec3(flag.location))

			#check to see if he's made it to pirate flag
			if (flag.location - self.entity.location).length() < 20:
				kb.pather = None
				self.entity.drop()
				kb.world.treasuresTaken += 1
				#increment treasures captured count



class GrabTreasure(Behavior):
	def update(self, kb, message_queue, timedelta):
		if self.entity.isHoldingSomething(): return
		chest_infos = kb.infoForAllKnownOfType("treasurechest")
		if len(chest_infos) == 0: return
		treasure = kb.returnClosestThing(self.entity, chest_infos)
		if treasure.ent.owner != None:
			return
		if (treasure.location - self.entity.location).length() < 20:
			self.entity.grab(treasure.ent)
			kb.pather = None
			kb.target = None




class _FindEnemy(Behavior):
	def __init__(self, ent, typename):
		Behavior.__init__(self, ent )
		self.targetType = typename

	def update( self, kb, message_queue, timedelta):
		#if self.entity.isHoldingSomething() and isinstance( kb.target, Ents.Actor ):
		#	if (kb.target.location - self.entity.location) > 800:
		#		kb.target = None
		if kb.target is None and kb.isHoldingThingOfType("crate"):
			all_enemies =  kb.infoForAllKnownOfType(self.targetType)
			if len(all_enemies) == 0: return
			# THE FOLLOWING LINE WAS FAGGED UP BY IAN ON DECEMBER 4TH 2006
			enemies = [ x for x in all_enemies if x.ent.knowledge.isHoldingThingOfType('treasurechest') ]
			if len(enemies) == 0:
				enemies = all_enemies
			target = enemies[0]
			for n in enemies:
				if (n.ent.location - self.entity.location).length() < (target.ent.location-self.entity.location).length():
					target = n
			# don't retarget same enemy
			if (target.location-self.entity.location).length() > 800:
				return

			#Make sure ninja isn't too far out
			#if (target.location - self.entity.location).length() < 500:
			print "Found enemy! ", target.typeName
			kb.target = target.ent

			#set the pather to lock onto it
			kb.pather = PathFinder(self.entity)
			kb.pather.setup( vec3(kb.target.location), kb.target )

class _AttackTarget(Behavior):
	def __init__(self, ent, typename):
		Behavior.__init__(self, ent )
		self.targetType = typename
		self.throwReady = False


	def update(self, kb, message_queue, timedelta):
		#Grab the target and make sure it's of the right type
		target = kb.target#kb.infoForTarget()
		if target == None: return
		if target.typeName != self.targetType:
			return

		#figure out distance to target
		distance_vector = target.location - self.entity.location
		self.entity.faceTowards( distance_vector)

		#throw stuff at the target (if we're holding something)
		if self.entity.isHoldingSomething() and distance_vector.length()<100:
			kb.pather = None
			self.throwReady = True

		#wait till not moving to throw
		if self.entity.velocity.length() < 5 and self.throwReady:
			self.entity.throwAt( target)
			kb.target = None
			self.throwReady = False


class AttackNinja(_AttackTarget):
	def __init__(self, ent): _AttackTarget.__init__(self, ent, "ninja")

class FindNinja(_FindEnemy):
	def __init__(self, ent): _FindEnemy.__init__(self, ent, "ninja")

class AttackPirate(_AttackTarget):
	def __init__(self, ent): _AttackTarget.__init__(self, ent, "pirate")

class FindPirate(_FindEnemy):
	def __init__(self, ent): _FindEnemy.__init__(self, ent, "pirate")

	#SPOT OF EOINS SHAME
