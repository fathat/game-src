import array
from cgkit.cgtypes import vec3, mat4
import MathUtil

class Heightmap(object):
	def __init__(self, width=1, height=1 ):
		self.width      = width
		self.height     = height
		self.heightData = [0] * (self.width*self.height)

	def readHeightValues( self, heightValues ):
		self.heightData = heightValues

	def sizePlusOne( self ):
		return self.resize( self.width+1, self.height+1 )

	def resize( self, new_width, new_height ):
		return self.submap( 0, 0, new_width, new_height )

	def submap( self, sx, sz, new_width, new_height ):
		new_heightmap = Heightmap( new_width, new_height )
		for y in range( sz, sz+new_height ):
			for x in range( sx, sx+new_width):
				new_heightmap.setHeightSafe( x-sx, y-sz, self.getHeightSafe( x, y ) )
		return new_heightmap

	def subdivide(self, splits):
		patch_w = ((self.width-1) / splits) + 1
		patch_h = ((self.height-1) / splits) + 1
		grids = []
		for y in range(splits):
			for x in range(splits):
				grids.append( self.submap( x*(patch_w-1), y*(patch_h-1), patch_w, patch_h))
		return grids

	def getHeight( self, x, z ):
		return self.heightData[x+z*self.width]

	def setHeight( self, x, z, h ):
		self.heightData[x+z*self.width] = h

	def getHeightSafe( self, x, z ):
		if x < 0: x = 0
		if z < 0: z = 0
		if x >= self.width: x  = self.width-1
		if z >= self.height: z = self.height-1
		return self.getHeight( x, z )

	def setHeightSafe( self, x, z, h):
		if x < 0: x = 0
		if z < 0: z = 0
		if x >= self.width: x  = self.width-1
		if z >= self.height: z = self.height-1
		self.setHeight( x, z, h )

	def getTriangleAtPosition( self, x, y ):
		triangle = []
		ix = int(x)
		iy = int(y)
		balance = (x-ix)+(y-iy)
		if balance < 1:
			triangle.append( vec3( ix, self.getHeightSafe( ix, iy ), iy ) )
			triangle.append( vec3( ix, self.getHeightSafe( ix, iy+1 ), iy+1 ) )
			triangle.append( vec3( ix+1, self.getHeightSafe( ix+1, iy ), iy ) )
		else:
			triangle.append( vec3( ix, self.getHeightSafe( ix, iy+1 ), iy+1 ) )
			triangle.append( vec3( ix+1, self.getHeightSafe( ix+1, iy+1 ), iy+1 ) )
			triangle.append( vec3( ix+1, self.getHeightSafe( ix+1, iy ), iy ) )
		return triangle

	def getTrianglesAt( self, x, y ):
		ix = int(x)
		iy = int(y)
		triangle = (vec3( ix, self.getHeightSafe( ix, iy ), iy ),
					vec3( ix, self.getHeightSafe( ix, iy+1 ), iy+1 ),
					vec3( ix+1, self.getHeightSafe( ix+1, iy ), iy ),
					vec3( ix, self.getHeightSafe( ix, iy+1 ), iy+1 ),
					vec3( ix+1, self.getHeightSafe( ix+1, iy+1 ), iy+1 ),
					vec3( ix+1, self.getHeightSafe( ix+1, iy ), iy ))
		return triangle


	def normalAt( self, x, y ):
		t = self.getTriangleAtPosition( x, y )
		return MathUtil.getTriangleNormal( *t )

	def correctXY( self, x, y ):
		if x < 0:
			x = 0
		elif x > self.width-1:
			x = self.width-1
		if y < 0:
			y = 0
		elif y > self.height-1:
			y = self.height-1
		return x,y

	def getInterpolatedHeight( self, fx, fy ):
		x, y = self.correctXY( fx, fy )
		t = self.getTriangleAtPosition( x, y )

		#calculate the triangle's plane
		plane = MathUtil.getTrianglePlane( t[0], t[1], t[2] )
		return MathUtil.intersectRayWithPlane( plane, vec3( x,0,y), vec3(0,1,0 ) )



def fromFile( filename ):
	"""Creates a heightmap by loading a file"""
	fileobj = open( filename, mode='rb' )

	#read the header data
	headervalues = array.array('i')
	headervalues.read( fileobj, 2 )
	width  = headervalues[0]
	height = headervalues[1]

	hmap = Heightmap( width, height )

	#read the actual heightmap data
	binvalues = array.array('f')
	binvalues.fromfile( fileobj, width*height )
	hmap.readHeightValues( binvalues.tolist() )
	fileobj.close()
	return hmap

if __name__ == '__main__':
	h = fromFile( "hmap.bin" )
	print h.getInterpolatedHeight(4.5,4.5)
