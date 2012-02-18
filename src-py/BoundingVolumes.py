from cgkit.cgtypes import *
import OpenGL.GL as GL
import OpenGL.GLU as GLU
import MathUtil

class BoundingSphere(object):
	quadratic = GLU.gluNewQuadric()
	def __init__(self, center, radius):
		self.center = center
		self.radius = radius

	def __str__(self):
		return "Center: %s, Radius: %s" % ( str(self.center), str(self.radius) )

	def transformedBy( self, transform_matrix ):
		return BoundingSphere( MathUtil.transformPoint(self.center, transform_matrix), self.radius )

	def transformedTo( self, position, yaw, pitch, roll ):
		m = MathUtil.buildTransformMatrix( position, yaw, pitch, roll)
		return self.transformedBy( m )

	def draw(self):
		GL.glPushMatrix()
		GL.glTranslatef( self.center.x, self.center.y, self.center.z )
		GL.glScalef( self.radius, self.radius, self.radius )
		GL.glPolygonMode( GL.GL_FRONT_AND_BACK,  GL.GL_LINE )
		GL.glDisable( GL.GL_TEXTURE_2D)
		GLU.gluSphere( self.quadratic, 1.0, 24, 24 )
		GL.glEnable( GL.GL_TEXTURE_2D)
		GL.glPolygonMode( GL.GL_FRONT_AND_BACK,  GL.GL_FILL )
		GL.glPopMatrix()

def distance(point_a, point_b ):
	return (point_b - point_a).length()

def findCenter(point_list):
	cx,cy,cz = 0,0,0
	for point in point_list:
		cx += point[0]
		cy += point[1]
		cz += point[2]
	sz = len(point_list)
	return vec3( float( cx/sz), float( cy/sz), float(cz/sz) )

def findFarthestDistanceFrom( center, point_list):
	d = 0
	for point in point_list:
		v = point - center
		d = max(d, v.length())
	return d

def createSphereAroundPoints(point_list):
	center = findCenter(point_list)
	return BoundingSphere( center, findFarthestDistanceFrom( center, point_list) )

def createSphereAroundSpheres( sphere_list ):
	sphere_centers = [ x.center for x in sphere_list ]
	center = findCenter(sphere_centers)
	max_distance = 0
	for sphere in sphere_list:
		max_distance = max( distance( sphere.center, center ) + sphere.radius, max_distance )
	return BoundingSphere(center, max_distance)

def testVolumes():
	point_list = [
		vec3( 2,2,2 ),
		vec3( 1,1,1 ),
		vec3( -10, 0, 0 )
	]
	print createSphereAroundPoints( point_list )

if __name__== '__main__':
	testVolumes()
