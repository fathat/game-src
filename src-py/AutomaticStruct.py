class Struct(object):
	"""A structure that can be initialized by passing keyword parameters to the constructor"""
	def __init__(self, *args,**kwargs):
		"""Accepts a list of keyword parameters to fill out values in the structure"""
		assert len(args) == 0
		for (key,value) in kwargs.items():
			setattr(self, key, value)