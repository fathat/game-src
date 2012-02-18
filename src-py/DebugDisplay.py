
class DebugEntry(object):
	"""An entry in the DebugInfo block. Contains some value"""
	def __init__(self, category, name, value, updateDelay=0 ):
		self.category    = category
		self.name        = name
		self.value 		 = value
		self.currentCountdown = updateDelay
		self.updateDelay = updateDelay

	def step( self, timestep ):
		self.currentCountdown = max( self.currentCountdown - timestep, 0 )

	def update( self, newvalue ):
		if self.currentCountdown > 0: return
		self.value = newvalue
		self.currentCountdown = self.updateDelay

	def __str__(self):
		if not len(self.category):
			return "%s: %s" % ( self.name, self.value )
		else:
			return "[%s] %s: %s" % ( self.category, self.name, self.value )

class DebugInfoState(object):
	"""Contains all the debugging info values"""
	def __init__(self):
		self.info = {}

	def step(self, timestep):
		for key,value in self.info.items():
			value.step( timestep)

	def setdelay( self, name, category="",  delay=1 ):
		key = (category,name)
		self.info[key].updateDelay = delay

	def update( self, name, value, category="", delay=0 ):
		key = (category,name)
		if key in self.info:
			self.info[key].update( value )
		else:
			self.info[key] = DebugEntry( category, name, value, delay )

	def __str__(self):
		return "\n".join( [ str(x) for x in self.info.values()] )

	def draw( self, font, x=0, y=0 ):
		my_x = x
		my_y = y

		for debug_entry in self.info.values():
			w,h = font.draw( my_x, my_y, str(debug_entry.name), static=True )
			font.draw(my_x+w, my_y, str(debug_entry.value), static=False )
			my_y += font.size + h



globalState = DebugInfoState()

def get(name, category=""):
	return globalState.info[(category,name)]

def update( name, value, category="", delay=0 ):
	globalState.update( name, value, category, delay )

def step( timestep ):
	globalState.step(timestep)


if __name__ == '__main__':
	d = DebugInfoState()
	d.update( "lol", 5, delay=1 )
	d.update( "lol", 5.6 )
	d.update( "lollerskates", 5 )
	d.step( 1 )
	d.update( "lol", 5.7 )
	print str(d)


