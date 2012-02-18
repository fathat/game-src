from OpenGL.GL import *
import glew

activeShader = None

class Shader(object):
	"""An OpenGL shader object"""
	def __init__( self, shader_type, name="(unnamed shader)" ):
		self.shaderObject = glew.glCreateShaderObjectARB( shader_type )
		self.name = name
		self.program = None
	
	def __del__ (self ):
		print "ARRR!"
		if self.program:
			self.program.detachShader( self )
			self.program = None
		
		glDeleteObjectARB( self.shaderObject )
		print "Shader " + self.name + " deleted"
	
	def source( self, source_string ):
		glew.glShaderSourceARB( self.shaderObject, len(source_string), source_string )
	
	def compileShader( self ):
		glew.glCompileShaderARB( self.shaderObject )
		if glew.glGetObjectParameterivARB ( self.shaderObject, glew.GL_OBJECT_COMPILE_STATUS_ARB ):
			print "%s compiled successfuly." % (self.name)
		else:
			print "Compile failed on shader %s: " % (self.name)
			self.printInfoLog( ) 
	
	
	def infoLog( self ):
		return glew.glGetInfoLogARB (self.shaderObject )
	
	def printInfoLog( self ):
		print self.infoLog()

class UniformVar(object):
	def __init__(self, set_function, name, value ):
		self.setFunction = set_function
		self.name = name
		self.value = value
	
	def set(self):
		self.setFunction( self.name, self.value )

class Program( object ):
	"""An OpenGL shader program"""
	def __init__(self):
		self.programObject = glew.glCreateProgramObjectARB()
		self.shaders = []
		self.uniformVars = {}
	
	def __del__(self):
		glew.glDeleteObjectARB( self.programObject) 
	
	def attachShader( self, shader ):
		self.shaders.append( shader )
		shader.program = self
		glew.glAttachObjectARB( self.programObject, shader.shaderObject )
	
	def detachShader( self, shader ):
		self.shaders.remove( shader )
		glew.glDetachObjectARB( self.programObject, shader.shaderObject )
		print "Shader detached"
	
	def link( self ):
		glew.glLinkProgramARB( self.programObject )
	
	def use( self ):
		global activeShader
		activeShader = self
		glew.glUseProgramObjectARB( self.programObject )
		self.setVars()
		
	
	def stop(self):
		global activeShader
		glew.glUseProgramObjectARB( 0 )
		activeShader = None
	
	def uniform1i( self, name, value ):
		self.uniformVars[name] = UniformVar( self.set_uniform1i, name, value )
		if self == activeShader:
			self.uniformVars[name].set()
	
	def set_uniform1i( self, name, value ):
		location = glew.glGetUniformLocationARB( self.programObject, name )
		glew.glUniform1iARB( location, value )
	
	def setVars(self):
		for name, var in self.uniformVars.iteritems():
			var.set()
	
	def printInfoLog( self ):
		print glew.glGetInfoLogARB (self.programObject )


def MakePixelShaderFromSource ( src ):
	return MakeShaderFromSource( src, glew.GL_FRAGMENT_SHADER_ARB )

def MakeVertexShaderFromSource ( src ):
	return MakeShaderFromSource( src, glew.GL_VERTEX_SHADER_ARB )

def MakeShaderFromSource( src, shader_type ):
		shader =  Shader( shader_type )
		shader.source( src )
		shader.compileShader()
		return shader

def MakeProgramFromSourceFiles( vertex_shader_name, pixel_shader_name ):
	file = open( vertex_shader_name, "r")
	vs_src = file.tostring()
	file.close()
	file = open( pixel_shader_name, "r")
	ps_src = file.tostring()
	file.close()
	return MakeProgramFromSource( vs_src, ps_src )

def MakeProgramFromSource( vertex_shader_src, pixel_shader_src ):
	vs = MakeVertexShaderFromSource( vertex_shader_src )
	ps = MakePixelShaderFromSource ( pixel_shader_src )
	
	p = Program()
	p.attachShader( vs )
	p.attachShader( ps )
	p.link()
	p.use()
	return p

def DisableShaders():
	global activeShader 
	glew.glUseProgramObjectARB( 0 )
	activeShader = None
