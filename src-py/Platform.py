import os
import time

def isUnix():
    return os.name == 'posix'

### Functions for converting paths to proper platform specific paths
def asUnixPath( aPath ):
	"""Takes in a path and ensures that it's correct for the platform"""
	if aPath == None: return None
	#if '\\' in str(aPath) and __debug__:
	#	print 'Warning: Windows path seperator found in path:', str(aPath)
	newPath = str(aPath).replace('\\', '/' )
	return newPath

def asWindowsPath( aPath ):
	return aPath

if os.name == 'nt':
	asOSPath = asWindowsPath
else:
	asOSPath = asUnixPath


#Timing functions
if isUnix():
	time_in_seconds = time.time
else:
	time_in_seconds = time.clock


#Custom segfault handler (for when C libraries crash, mostly)
def _segfaultHandler(signum, frame):
    exit()
    #print signum
    #print frame
    #print "Segfault Detected"
    #raise SegFault()

class SegFault(Exception):
    pass

#if isUnix():
#    #Installs a custom segfault handler 
#    import signal
#    signal.signal(signal.SIGSEGV, _segfaultHandler)

