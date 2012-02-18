import sys
import time
import newton
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from AppGL import *
from listmatrix import *

#Callback function which causes the body to fall downwards
def FallDown( body ):
	mass, ix, iy, iz = body.GetMassMatrix()
	body.SetForce( (0, -mass*9.8, 0) )

#Callback function which causes the body to fall upwards
def FallUp ( body ):
	mass, ix, iy, iz = body.GetMassMatrix()
	body.SetForce( (0, mass*9.8, 0) )

#Callback function for changes in autoactivation state stuff
def Autoactive( body, state ):
	pass
	#print str(state )


#This doesn't do anything interesting, it's just to show how you can
#subclass newton.Body and override some of its virtual functions
class FloorBody( newton.Body ):
	count = 0 # counts how many times the floor has been hit by something
	def OnTreeCollisionWith(self, body):
		self.count = self.count + 1


#A box! That falls!
class FallingBox(object):
	def __init__(self, world, startingPosition, forceFunc):
		self.world = world
		collision  = newton.Box( self.world, 2, 2, 2, None );
		body       = newton.Body( self.world, collision )

		#Set callbacks here
		body.ApplyForceAndTorqueCallback = forceFunc
		body.AutoactiveCallback = Autoactive
		body.TransformCallback  = self.OnTransform

		#Set the location of the body
		body.SetMassMatrix( 1.0, 1.0, 1.0, 1.0 )
		x, y, z = startingPosition
		matrix = TranslationMatrix( x, y, z )
		body.SetMatrix( matrix )

		#(for debugging purposes, to make sure garbage collector isn't causing problems)
		del body

	def OnTransform(self, body, matrix ):
		self.matrix = matrix

	def Draw( self ):
		m = self.matrix
		glPushMatrix()
		glMultMatrixf( m )
		DrawGLBox(1, 1, 1)
		glPopMatrix()


#Callback for material. Doesn't actually do anything.
def BeginCallback( material, body1, body2, userdata ):
	print "Oh, hi there"
	print material

#Callback for material. Doesn't actually do anything.
def ProcessCallback ( material, contact, userdata ):
	print material

def EndCallback( material, userdata ):
	print material
	print "end"

#Main app class for this example
class NewtonTest (AppGL):
	def OnInit( self ):
		self.sphere = gluNewQuadric()
		self.InitPhysics()
		self.timeDelta = 0
		self.lastTime = time.clock()
		texture = LoadTexture("crate.bmp")
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	def InitPhysics( self ):
		self.world = newton.World()
		self.world.MaterialSetCollisionCallback( 0, 0, lambda: "Hi", BeginCallback, ProcessCallback, EndCallback )
		#print "Default group ID: " + str( self.world.Materials.GetDefaultGroupID() )

		self.boxes = []
		for x in range( 3 ):
			for z in range ( 3 ):
				for y in range ( 3 ):
					self.boxes.append( FallingBox( self.world, ( x*4, y*4, z*4 ), FallDown ) )

		#This creates a nifty tree collision with two triangles for the floor
		print "Making floor box"
		self.floorBox = newton.TreeCollision( self.world )
		self.floorBox.BeginBuild()
		self.floorBox.AddFace( [-50, 0, -50,
								 50, 0,  50,
								 50, 0, -50], 1 )

		self.floorBox.AddFace( [ -50, -5,-50,
								 -50, -5, 50,
								  50, -5, 50 ], 0 )
		self.floorBox.EndBuild( 1 )
		self.floorBody = FloorBody( self.world, self.floorBox )
		self.floorBody.SetMatrix( TranslationMatrix( 0, -5, 0 ) )
		print "Done initializing physics"

	def Update ( self ):
		t = time.clock()
		timeDelta = t - self.lastTime
		self.lastTime = t
		self.world.Update(timeDelta)
		self.DrawScene()

	def DrawScene ( self ):
		glEnable( GL_TEXTURE_2D)
		glClearColor( 0.5, 0.5, 0.5, 1 )
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glLoadIdentity()

		glPushMatrix()

		#Draw all the boxes
		glTranslatef(-8, 0, -60 )
		for b in self.boxes:
			b.Draw()

		#Draw the floor
		glTranslatef(0, -5, 0 )
		glBegin( GL_TRIANGLES )
		glTexCoord2f( 0.0, 0.0 )
		glVertex3f( -50, 0, -50 )
		glTexCoord2f( 1.0, 1.0 )
		glVertex3f(  50, 0,  50 )
		glTexCoord2f( 1.0, 0.0 )
		glVertex3f(  50, 0, -50 )

		glTexCoord2f( 0.0, 0.0 )
		glVertex3f( -50, -5, -50 )
		glTexCoord2f( 1.0, 0.0 )
		glVertex3f(  -50, -5,  50 )
		glTexCoord2f( 1.0, 1.0 )
		glVertex3f(  50, -5, 50 )
		glEnd()
		glPopMatrix()
		glutSwapBuffers()


if __name__ == '__main__':
	nt = NewtonTest(1024, 768, 'secret opengl window')
	nt.MainLoop()
