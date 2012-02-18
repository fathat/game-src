
class ProjectileManager(object):
	def __init__(self, world):
		#classify projectiles by their drawables
		self.projectiles = {}
	
	def draw(self):
		pass


class Projectile(object):
	def __init__(self, drawable, mass=1.0, damage=0.0 ):
		self.drawable = drawable
		self.damage = damage
		self.mass = mass
	
	def draw(self):
		self.drawable.draw()
		pass