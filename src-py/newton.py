# The purpose of this module is essentially to subclass the basic SWIG generated
# pynewton classes and add a bit of functionality to them (mostly callback related
# stuff). This could be done in the SWIG interface file, but it's easier to do it
# here since it makes adding python-specific extensions to newton easier.
import pynewton

try:
	import OpenGL.GL as GL
	import OpenGL.GLU as GLU
	GLPresent = True
except:
	GLPresent = False


def GetEulerAngle ( matrix ):
	return pynewton.GetEulerAngle( matrix )

def SetEulerAngle ( angle ):
	return pynewton.SetEulerAngle( angle )

#extensions to body
def NullApplyForceAndTorqueCallback( body ) :
	pass

def NullTransformCallback( body, matrix ):
	pass

def NullAutoactiveCallback( body, state ):
	pass

def NullBodyDestructCallback( body ):
	pass

class Body( pynewton.Body ):
	def __init__( self, world, cg ):
		self.ApplyForceAndTorqueCallback = None
		self.TransformCallback = None
		self.AutoactiveCallback = None
		self.DestructorCallback = None
		self.TreeCollisionCallback = None

		pynewton.Body.__init__(self, world, cg )
		world.RegisterBody( self )
		self.py_cg = cg;

	def SetApplyForceAndTorqueCallback( self, callback ):
		self.ApplyForceAndTorqueCallback = callback

	def SetAutoactiveCallback( self, callback ):
		self.AutoactiveCallback = callback

	def GetCollision( self ):
		return self.py_cg

	def OnApplyForceAndTorque(self):
		if self.ApplyForceAndTorqueCallback != None:
			self.ApplyForceAndTorqueCallback( self )

	def OnAutoactive(self, state ):
		if  self.AutoactiveCallback != None:
			self.AutoactiveCallback( self, state )

	def OnTransform( self ):
		matrix = self.GetMatrix()
		if self.TransformCallback != None:
			self.TransformCallback( self, matrix )

	def OnDestruct( self ):
		if self.DestructorCallback != None:
			self.DestructorCallback( self, matrix )

	def OnTreeCollisionWith( self, body ):
		if self.TreeCollisionCallback != None:
			self.TreeCollisionCallback(body)

	def Draw( self ):
		m = self.GetMatrix()
		if not GLPresent: raise "OpenGL module not loaded, cannot draw"
		GL.glPushMatrix()
		GL.glMultMatrixf( m )
		c = self.GetCollision()
		c.draw()
		GL.glPopMatrix()



class _materialCallback( object ):
	def __init__(self, id1, id2, begin_function, process_function, end_function, userobject):
		self.id1 = id1
		self.id2 = id2
		self.beginCallback   = begin_function
		self.processCallback = process_function
		self.endCallback     = end_function
		self.userobject = userobject


#extensions to world
class World( pynewton.World ):
	def __init__(self ):
		self.bodyList = []
		self.newtonBodyLookup = {}
		self.materialCallbacks = {}
		self.currentCallback = None
		self.raycastUserData = None
		self.raycastCallback = None
		pynewton.World.__init__(self)

	def RegisterBody( self, body ):
		self.bodyList.append( body )
		self.newtonBodyLookup[body.IDKey()] = body

	def UnregisterBody( self, body ):
		self.bodyList.remove( bodyList.index(body) )
		del self.newtonBodyLookup[body.m_body]

	def NewtonBodyToBody( self, ptr ):
		return self.newtonBodyLookup[int(ptr)]

	def ForEachBodyDo( self, function ):
		for body in self.bodyList:
			function( body )
	
	def RayCast( self, p0, p1, callback, userdata):
		"""Casts a ray in the world defined by p0 and p1 and calls callback
		with the body, normal, collision id, user data and intersection distance"""
		self.raycastUserData = userdata
		self.raycastCallback = callback
		self.CppRayCast.__call__(p0[0], p0[1], p0[2], p1[0], p1[1], p1[2])
	
	def RayCastCallback( self, body, nx, ny, nz, collisionID, intersectParam ):
		#delegate this off to the user specified function
		return self.raycastCallback( body, (nx, ny, nz), collisionID, self.raycastUserData, intersectParam )

	def MaterialSetCollisionCallback( self, id1, id2, userdata=None, begin_func=None, process_func=None, end_func=None ):
		self.materialCallbacks[(id1,id2)]  = _materialCallback( id1, id2, begin_func, process_func, end_func, userdata)
		self.RegisterMaterialCallbackBetween( id1, id2)

	def GetMaterialCallback(self, material, body1, body2):
		id1 = body1.MaterialGroupID()
		id2 = body2.MaterialGroupID()
		cb = self.materialCallbacks[(id1,id2)]
		return cb


	def MaterialBeginCollision( self, material, b1, b2 ):
		body1 = self.newtonBodyLookup[int(b1)]
		body2 = self.newtonBodyLookup[int(b2)]
		self.currentCallback = self.GetMaterialCallback( material, body1, body2 )
		if self.currentCallback.beginCallback:
			self.currentCallback.beginCallback(material,
											   body1,
											   body2,
											   self.currentCallback.userobject )

	def MaterialProcessCollision( self, material, contactHandle ):
		if self.currentCallback.processCallback:
			self.currentCallback.processCallback(material,
												 contactHandle,
												 self.currentCallback.userobject )

	def MaterialEndCollision( self, material ):
		if self.currentCallback.endCallback:
			self.currentCallback.endCallback( material,
											 self.currentCallback.userobject )

#collision extensions
class CollisionGeometry( pynewton.CollisionGeometry ):
	def draw(self):
		if not GlPresent: raise "OpenGL module could not be loaded"

class Sphere ( pynewton.Sphere ):
	def __init__(self, world, w, h, d, offset_matrix=None):
		pynewton.Sphere.__init__( self, world, w, h, d, offset_matrix )
		self.width  = w
		self.height = h
		self.depth  = d
		if GLPresent:
			self.quad   = GLU.gluNewQuadric()

	def draw(self):
		if not GLPresent: raise "OpenGL module could not be loaded"
		GL.glPushMatrix()
		GL.glScalef( self.width, self.height, self.depth )
		GL.glPolygonMode( GL.GL_FRONT_AND_BACK,  GL.GL_LINE )
		GLU.gluSphere( self.quad, 1.0, 12, 12 )
		GL.glPolygonMode( GL.GL_FRONT_AND_BACK,  GL.GL_FILL )
		GL.glPopMatrix()



class Box ( pynewton.Box ):
	pass

class Cone ( pynewton.Cone ):
	pass

class Cylinder (pynewton.Cylinder):
	pass

class ChamferCylinder (pynewton.ChamferCylinder):
	pass

class ConvexHull  (pynewton.ConvexHull):
	pass

class ConvexHullModifier  (pynewton.ConvexHullModifier):
	pass

class NullCollider (pynewton.NullCollider ):
	pass

class TreeCollision (pynewton.TreeCollision):
	pass

class TreeCollisionUserCallback ( pynewton.TreeCollisionUserCallback ):
	def __init__( self, func ):
		self.callbackFunc = func

	def OnCallback (self,  bodyWithTreeCollision, body, vertices, vertexstrideInBytes, indexCount, indexArray):
		if self.callbackFunc != None:
			self.callbackFunc( bodyWithTreeCollision, body, vertices, vertexstrideInBytes, indexCount, indexArray)
		pass

#material extensions
class Material ( pynewton.Material ):
	pass


#joint extensions
class BallJoint ( pynewton.BallJoint ):
	def __init__(self, *args, **kwargs ):
		self.callback = None
		return pynewton.BallJoint.__init__(self, *args, **kwargs )

	def OnCallback():
		if self.callback != None:
			self.callback( )
		pass

class Hinge ( pynewton.Hinge ):
	def __init__(self, *args, **kwargs ):
		self.callback = None
		return pynewton.Hinge.__init__( *args, **kwargs )

	def OnCallback( desc ):
		if self.callback != None:
			return self.callback( desc )
		return 0

class Slider ( pynewton.Slider ):
	def __init__( self, *args, **kwargs ):
		self.callback = None
		return pynewton.Slider.__init__( self, *args, **kwargs )

#	def OnCallback( desc ):
#		if self.callback != None:
#			return self.callback( desc )
#		return 0

class Corkscrew ( pynewton.Corkscrew ):
	def __init__(self, *args, **kwargs ):
		self.callback = None
		pynewton.Corkscrew.__init__(self, *args, **kwargs )

	def OnCallback( desc ):
		if self.callback != None:
			return self.callback( desc )
		return 0

class UniversalJoint ( pynewton.UniversalJoint ):
	def __init__(self, *args, **kwargs ):
		self.callback = None
		return pynewton.UniversalJoint.__init__( self, *args, **kwargs )

	def OnCallback( desc ):
		if self.callback != None:
			return self.callback( desc )
		return 0

class UpVector ( pynewton.UpVector ):
	def __init__(self, *args, **kwargs ):
		self.callback = None
		return pynewton.UpVector.__init__(self, *args, **kwargs )

	def OnCallback():
		if self.callback != None:
			self.callback( )


class Tire ( pynewton.Tire ):
	pass

class Vehicle ( pynewton.Vehicle ):

	def __init__(self, *args, **kwargs ):
		self.tires = []
		self.UpdateTireCallback = None
		return pynewton.Vehicle.__init__(self, *args, **kwargs )

	def AddTire ( self, matrix, pin, mass, width, radius, suspensionShock, suspensionSpring, suspensionLength, userData, collisionID):
		tire = pynewton.Vehicle.AddTire(self, matrix, pin, mass, width, radius, suspensionShock, suspensionSpring, suspensionLength, userData, collisionID)
		tires.append( tire )
		return tire

	def RemoveTire( self, tire ):
		del tires[tires.index(tire)]
		tire = pynewton.Vehicle.RemoveTire( self, tire )

	def OnCallback( self):
		if self.UpdateTireCallback != None:
			self.UpdateTireCallback(self)

#Heightmap
class HeightField ( pynewton.HeightField ):
	pass
