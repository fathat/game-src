# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _cmath
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class Plane(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Plane, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Plane, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _cmath.Plane_a_set
    __swig_getmethods__["a"] = _cmath.Plane_a_get
    if _newclass:a = property(_cmath.Plane_a_get, _cmath.Plane_a_set)
    __swig_setmethods__["b"] = _cmath.Plane_b_set
    __swig_getmethods__["b"] = _cmath.Plane_b_get
    if _newclass:b = property(_cmath.Plane_b_get, _cmath.Plane_b_set)
    __swig_setmethods__["c"] = _cmath.Plane_c_set
    __swig_getmethods__["c"] = _cmath.Plane_c_get
    if _newclass:c = property(_cmath.Plane_c_get, _cmath.Plane_c_set)
    __swig_setmethods__["d"] = _cmath.Plane_d_set
    __swig_getmethods__["d"] = _cmath.Plane_d_get
    if _newclass:d = property(_cmath.Plane_d_get, _cmath.Plane_d_set)
    def __init__(self, *args): 
        this = _cmath.new_Plane(*args)
        try: self.this.append(this)
        except: self.this = this
    def normalize(*args): return _cmath.Plane_normalize(*args)
    def distanceToPoint(*args): return _cmath.Plane_distanceToPoint(*args)
    __swig_destroy__ = _cmath.delete_Plane
    __del__ = lambda self : None;
Plane_swigregister = _cmath.Plane_swigregister
Plane_swigregister(Plane)

class Frustrum(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Frustrum, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Frustrum, name)
    __repr__ = _swig_repr
    __swig_setmethods__["planes"] = _cmath.Frustrum_planes_set
    __swig_getmethods__["planes"] = _cmath.Frustrum_planes_get
    if _newclass:planes = property(_cmath.Frustrum_planes_get, _cmath.Frustrum_planes_set)
    def __init__(self, *args): 
        this = _cmath.new_Frustrum(*args)
        try: self.this.append(this)
        except: self.this = this
    def update(*args): return _cmath.Frustrum_update(*args)
    def sphereInFrustrum(*args): return _cmath.Frustrum_sphereInFrustrum(*args)
    __swig_destroy__ = _cmath.delete_Frustrum
    __del__ = lambda self : None;
Frustrum_swigregister = _cmath.Frustrum_swigregister
Frustrum_swigregister(Frustrum)



