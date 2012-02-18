from OpenGL.GL import *
from OpenGL.GLU import *
import glew
import array
from BaseTexture import Texture
# D = 0x44
# X = 0x58
# T = 0x54
# 1 = 0x31
# 3 = 0x33
# 5 = 0x35

DXT1 = 0x31545844
DXT3 = 0x33545844
DXT5 = 0x35545844



class NotDDSFileException( Exception ):
	def __init__(self, value ):
		self.value = value
	def __str__(self):
		return repr( self.value ) 

class DDPixelFormat(object):
	def __init__(self, f):
		a = array.array( 'L' )
		a.fromfile( f, 8 )
		self.size   = a[0]
		self.flags  = a[1]
		self.fourcc = a[2]
		self.rgb_bit_count = a[3]
		self.r_mask = a[4]
		self.g_mask = a[5]
		self.b_mask = a[6]
		self.a_mask = a[7]
		print self.size
	
	def getGLFormatAndFactor(self):
		fourcc = self.fourcc
		if fourcc == DXT1:
			return glew.GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 2, 3
		elif fourcc == DXT3:
			return glew.GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 4, 4
		elif fourcc == DXT5:
			return glew.GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 4, 4
		else:
			print "Texture format not recognized" 
		return None, None, None

class DDSImage(object):
	width  = 0
	height = 0
	components = 0
	format = None
	mipMapCount = None
	pixels = None # stored as a string
	def __init__(self, filename ):
		f = open(filename, 'rb')
		
		#read the first 4 bytes. They should come out to "DDS ". If they dont,
		#throw exception
		a = array.array( 'c' )
		a.fromfile( f, 4 )
		if a.tostring() != 'DDS ':
			raise NotDDSFileException( str( file) + " is not a valid DDS file" )
		
		#Read to the ddpfpixelformat structure (72 bytes in )
		#f.seek( 72, 1 ) 
		a = array.array('L')
		a.fromfile( f, 18 )
		self.height = a[2]
		self.width  = a[3]
		self.linearSize  = a[4]
		self.backBufferCount = a[5]
		self.mipMapCount = a[6]
		self.alphaBitDepth = a[7]
		self.reserved = a[8]
		self.surface  = a[9]
		
		print a 
		
		print a[0]
		print self.height
		print self.width
		print self.mipMapCount
		print self.alphaBitDepth
		print self.linearSize
		print ""
		
		#read the pixelformat structure
		pixformat = DDPixelFormat( f )
		self.format, factor, self.components = pixformat.getGLFormatAndFactor()
		
		#read past DDSCAPS2 and dwTextureStage
		f.seek(20 , 1 )
		
		#print f.tell()
		
		print "linear size:" + str(self.linearSize)
		buffersize = self.linearSize
		if self.mipMapCount > 1: buffersize = self.linearSize * factor
		
		#read the pixels
		
		#pixarray = array.array( 'c' )
		#pixarray.fromfile( f, buffersize )
		print buffersize
		self.pixels = f.read( buffersize)
		

class DDSTexture(Texture):
	def __init__(self, filename ):
		imgData = DDSImage(filename)
		blockSize = 16
		if imgData.format == glew.GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			blockSize = 8
		self.id = glGenTextures(1)
		glBindTexture( GL_TEXTURE_2D, self.id )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		self.width  = imgData.width
		self.height = imgData.height
		w = self.width
		h = self.height
		offset = 0
		for miplevel in range( imgData.mipMapCount ):
			sz = ((w+3)/4) * ((h+3)/4) * blockSize
			glew.glCompressedTexImage2DARB(GL_TEXTURE_2D, miplevel, imgData.format, w, h, 0, sz, imgData.pixels[offset:] ) 
			w = max( w/2, 1 ) 
			h = max( h/2, 1 )
			offset += sz
