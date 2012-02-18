import MathUtil
from cgkit.cgtypes import *
from LangExt import make_property

class NotImplemented(Exception):
    pass

class ILocation(object):
    """Defines the interface for Location objects"""

    @make_property
    def x():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def y():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def z():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def yaw():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def pitch():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def roll():
        def fget(self): raise NotImplemented
        def fset(self, m): raise NotImplemented
        return locals()

    @make_property
    def quaternion():
        def fget(self): raise NotImplemented
        return locals()

    @make_property
    def matrix():
        def fget(self): raise NotImplemented
        return locals()

    @make_property
    def xyz():
        def fget(self): raise NotImplemented
        return locals()

    @make_property
    def ypr():
        def fget(self): raise NotImplemented
        return locals()


class StandardLocation(ILocation):
    def __init__(self, matrix=None):
        if matrix:
            self.matrix = matrix
        else:
            self._xyz = vec3()
            self._q4  = quat()

    @make_property
    def x():
        def fget(self): return self._xyz.x
        def fset(self, m): self._xyz.x = m
        return locals()

    @make_property
    def y():
        def fget(self): return self._xyz.y
        def fset(self, m): self._xyz.y = m
        return locals()

    @make_property
    def z():
        def fget(self): return self._xyz.z
        def fset(self, m): self._xyz.z = m
        return locals()

    @make_property
    def yaw():
        def fget(self): return self.ypr[0]
        return locals()

    @make_property
    def pitch():
        def fget(self): return self.ypr[1]
        return locals()

    @make_property
    def roll():
        def fget(self): return self.ypr[2]
        return locals()

    @make_property
    def quaternion():
        def fget(self): return self._q4
        def fset(self, m): self._q4 = m
        return locals()

    @make_property
    def matrix():
        def fget(self):
            return MathUtil.buildTransformMatrixQ(self._xyz, self._q4)
        def fset(self, m):
            translation, rotation, scaling = m.decompose()
            self._xyz = translation
            self._q4 = quat(rotation)
        return locals()

    @make_property
    def xyz():
        def fget(self):
            return self._xyz
        def fset(self, m):
            self._xyz = m
        return locals()

    @make_property
    def ypr():
        def fget(self):
            p, y, r = self._q4.toMat3().toEulerYXZ()
            return y, p, r
        return locals()


class DependentPhysicsLocation(ILocation):
    def __init__(self, body):
        self.body = body
        self.matrix = None


    def readMatrix(self):
        self.matrix = mat4(self.body.GetMatrix()).transpose()

    def writeMatrix(self):
        self.body.SetMatrix( self.matrix.toList() )
