from cgkit.cgtypes import mat4, mat3, vec3, vec4

def MatrixFrontVector( aMatrix ):
	return vec4(aMatrix.transpose().getColumn( 0 ))

def MakeRotationMatrix( yaw, pitch, roll ):
	base = mat4(1)
	myaw   = base.rotation( yaw,   vec3( 0, 1, 0 ) ) 
	mpitch = base.rotation( pitch, vec3( 1, 0, 0 ) )
	mroll  = base.rotation( roll,  vec3( 0, 0, 1 ) )
	return myaw * mpitch * mroll