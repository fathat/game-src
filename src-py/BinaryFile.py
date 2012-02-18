import array

def read( file, number, ctype ):
	a = array.array( ctype )
	a.fromfile( file, number )
	return a.tolist()

def readFromString( string_to_read, ctype ):
	a = array.array(ctype)
	a.fromstring( string_to_read )
	return a.tolist()

def readInts( file, number ):
	a = array.array( 'i' )
	a.fromfile( file, number )
	return a.tolist()

def readFloats( file, number ):
	a = array.array( 'f' )
	a.fromfile( file, number )
	return a.tolist()

def readString( file, number ):
	a = array.array('c')
	a.fromfile( file, number )
	rval = a.tostring()
	return rval[:rval.find('\0')]

