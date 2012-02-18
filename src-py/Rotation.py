import MathUtil
from cgkit.cgtypes import *

class Rotation(object):
    """A rotation!"""

    def __init__(self, *args):
        self.quaternion = quat(*args)

    @property
    def yaw(self):
        pass

    @property
    def pitch(self):
        pass

    @property
    def roll(self):
        pass

    @property
    def matrix(self):
        return self.quaternion.toMat4()
