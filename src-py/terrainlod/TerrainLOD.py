#!/usr/bin/env python
from cgkit.cgtypes import vec3, vec4, mat4
from Heightmap import Heightmap
import GlTest

class LODDesc(object):
	def __init__(self, north=-1, south=-1, east=-1, west=-1, me=-1):
		self.westLOD  = west
		self.eastLOD  = east
		self.northLOD = north
		self.southLOD = south
		self.lod = me

class TerrainIndexBlock(object):
	def __init__(self, dim):
		self.dim = dim
		self.simplificationLevel = {
			"west" : 1,
			"east" : 1,
			"south" : 1,
			"north" : 1
		}
		self.indices = []
		for y in xrange(self.dim-1):
			for x in xrange(self.dim-1):
				self.indices.append( self.getIndexForCoordinate( x, y ) )
				self.indices.append( self.getIndexForCoordinate( x, y+1 ) )
				self.indices.append( self.getIndexForCoordinate( x+1, y ) )
				self.indices.append( self.getIndexForCoordinate( x, y+1 ) )
				self.indices.append( self.getIndexForCoordinate( x+1, y+1 ) )
				self.indices.append( self.getIndexForCoordinate( x+1, y ) )

	def updateIndices(self):
		for y in xrange(self.dim-1):
			for x in xrange(self.dim-1):
				self.indices[i+0] = self.getIndexForCoordinate( x, y )
				self.indices[i+1] = self.getIndexForCoordinate( x, y+1 )
				self.indices[i+2] = self.getIndexForCoordinate( x+1, y )
				self.indices[i+3] = self.getIndexForCoordinate( x, y+1 )
				self.indices[i+4] = self.getIndexForCoordinate( x+1, y+1 )
				self.indices[i+5] = self.getIndexForCoordinate( x+1, y )

	def getIndexForCoordinate( self, xc, yc ):
		x = xc
		y = yc
		if self.dim > 2:
			if x==0 and self.simplificationLevel['west'] > 1:
				y -= y % self.simplificationLevel['west']
			if x== self.dim-1 and self.simplificationLevel['east'] != 0:
				y += self.simplificationLevel['east'] - (y % self.simplificationLevel['east'])
			if y==0 and self.simplificationLevel['north'] > 1:
				x -= x % self.simplificationLevel['north']
			if y == self.dim-1 and self.simplificationLevel['south'] != 0:
				x += self.simplificationLevel['south'] - (x % self.simplificationLevel['south'])
		return x+y*self.dim


class TerrainMap(object):
	def __init__(self, heightgrid, patch_size, spacing):
		self.heightgrid = heightgrid
		self.patchSize = patch_size
		self.spacing = spacing

		#subdivide the terrain into various patches
		self.dim = (self.heightgrid.width-1)/self.patchSize
		self.grids = self.heightgrid.subdivide( self.dim )

		#make a patch for each heightgrid
		self.patches = [TerrainPatch(self, x) for x in self.grids ]

	def arrangePatches(self):
		worldSize = (self.patchSize-1)*self.spacing
		for y in xrange(self.dim):
			for x in xrange(self.dim):
				self.patches[x+y*self.dim].column = x
				self.patches[x+y*self.dim].row = y
				self.patches[x+y*self.dim].offset = (worldSize*x), (worldSize*y)

	def drawAll(self, camera=None):
		for patch in self.patches:
			patch.draw()



class TerrainPatch(object):
	def __init__(self, map, heightgrid):
		self.map = map
		self.baseHeightGrid = heightgrid
		self.row    = 0
		self.column = 0
		self.offset = 0,0
		self.mipmaps = []

class TerrainMipMapLevel(object):
	def __init__(self, patch, heightgrid, spacing):
		self.baseHeightGrid = heightgrid
		self.spacing = spacing






