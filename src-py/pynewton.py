# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _pynewton

# This file is compatible with both classic and new-style classes.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


try:
    import weakref
    weakref_proxy = weakref.proxy
except:
    weakref_proxy = lambda x: x



new_CDoubleArray = _pynewton.new_CDoubleArray

delete_CDoubleArray = _pynewton.delete_CDoubleArray

CDoubleArray_getitem = _pynewton.CDoubleArray_getitem

CDoubleArray_setitem = _pynewton.CDoubleArray_setitem
class World(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, World, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, World, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ World instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewtonWorld world) -> World
        __init__(self) -> World

        1
        """
        if self.__class__ == World:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, World, 'this', _pynewton.new_World(*args))
        _swig_setattr(self, World, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_World):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def RegisterBody(*args):
        """
        RegisterBody(self, Body body)

        1
        """
        return _pynewton.World_RegisterBody(*args)

    def UnregisterBody(*args):
        """
        UnregisterBody(self, Body body)

        1
        """
        return _pynewton.World_UnregisterBody(*args)

    def GetNewtonWorld(*args):
        """
        GetNewtonWorld(self) -> NewtonWorld

        1
        """
        return _pynewton.World_GetNewtonWorld(*args)

    def Update(*args):
        """
        Update(self, dFloat timeStep)

        1
        """
        return _pynewton.World_Update(*args)

    def SetPlatformArchitecture(*args):
        """
        SetPlatformArchitecture(self, int mode)

        1
        """
        return _pynewton.World_SetPlatformArchitecture(*args)

    def SetSolverModel(*args):
        """
        SetSolverModel(self, int model)

        1
        """
        return _pynewton.World_SetSolverModel(*args)

    def SetFrictionModel(*args):
        """
        SetFrictionModel(self, int model)

        1
        """
        return _pynewton.World_SetFrictionModel(*args)

    def GetTimeStep(*args):
        """
        GetTimeStep(self) -> dFloat

        1
        """
        return _pynewton.World_GetTimeStep(*args)

    def SetMinimumFrameRate(*args):
        """
        SetMinimumFrameRate(self, dFloat frameRate)

        1
        """
        return _pynewton.World_SetMinimumFrameRate(*args)

    def SetWorldSize(*args):
        """
        SetWorldSize(self, dFloat minPoint, dFloat maxPoint)

        1
        """
        return _pynewton.World_SetWorldSize(*args)

    def GetVersion(*args):
        """
        GetVersion(self) -> int

        1
        """
        return _pynewton.World_GetVersion(*args)

    def RemoveBody(*args):
        """
        RemoveBody(self, Body body)

        1
        """
        return _pynewton.World_RemoveBody(*args)

    def SetUserData(*args):
        """
        SetUserData(self, PyObject data)

        1
        """
        return _pynewton.World_SetUserData(*args)

    def GetUserData(*args):
        """
        GetUserData(self) -> PyObject

        1
        """
        return _pynewton.World_GetUserData(*args)

    def RayCastCallback(*args):
        """
        RayCastCallback(self, NewtonBody body, dFloat nx, dFloat ny, dFloat nz, int collisionID, 
            dFloat intersectParam) -> dFloat

        1
        """
        return _pynewton.World_RayCastCallback(*args)

    def CppRayCast(*args):
        """
        CppRayCast(self, float ax, float ay, float az, float bx, float by, float bz)

        1
        """
        return _pynewton.World_CppRayCast(*args)

    def MaterialGetDefaultGroupID(*args):
        """
        MaterialGetDefaultGroupID(self) -> int

        1
        """
        return _pynewton.World_MaterialGetDefaultGroupID(*args)

    def MaterialCreateGroupID(*args):
        """
        MaterialCreateGroupID(self) -> int

        1
        """
        return _pynewton.World_MaterialCreateGroupID(*args)

    def MaterialDestroyAllGroupID(*args):
        """
        MaterialDestroyAllGroupID(self)

        1
        """
        return _pynewton.World_MaterialDestroyAllGroupID(*args)

    def MaterialSetDefaultSoftness(*args):
        """
        MaterialSetDefaultSoftness(self, int id0, int id1, dFloat value)

        1
        """
        return _pynewton.World_MaterialSetDefaultSoftness(*args)

    def MaterialSetDefaultElasticity(*args):
        """
        MaterialSetDefaultElasticity(self, int id0, int id1, dFloat elasticCoef)

        1
        """
        return _pynewton.World_MaterialSetDefaultElasticity(*args)

    def MaterialSetDefaultCollidable(*args):
        """
        MaterialSetDefaultCollidable(self, int id0, int id1, int state)

        1
        """
        return _pynewton.World_MaterialSetDefaultCollidable(*args)

    def MaterialSetContinuousCollisionMode(*args):
        """
        MaterialSetContinuousCollisionMode(self, int id0, int id1, int state)

        1
        """
        return _pynewton.World_MaterialSetContinuousCollisionMode(*args)

    def MaterialSetDefaultFriction(*args):
        """
        MaterialSetDefaultFriction(self, int id0, int id1, dFloat staticFriction, dFloat kineticFriction)

        1
        """
        return _pynewton.World_MaterialSetDefaultFriction(*args)

    def MaterialBeginCollision(*args):
        """
        MaterialBeginCollision(self, Material mat, NewtonBody b1, NewtonBody b2)

        1
        """
        return _pynewton.World_MaterialBeginCollision(*args)

    def MaterialProcessCollision(*args):
        """
        MaterialProcessCollision(self, Material mat, NewtonContact contactHandle)

        1
        """
        return _pynewton.World_MaterialProcessCollision(*args)

    def MaterialEndCollision(*args):
        """
        MaterialEndCollision(self, Material mat)

        1
        """
        return _pynewton.World_MaterialEndCollision(*args)

    def RegisterMaterialCallbackBetween(*args):
        """
        RegisterMaterialCallbackBetween(self, int id0, int id1)

        1
        """
        return _pynewton.World_RegisterMaterialCallbackBetween(*args)

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_World(self)
        return weakref_proxy(self)

class WorldPtr(World):
    def __init__(self, this):
        _swig_setattr(self, World, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, World, 'thisown', 0)
        self.__class__ = World
_pynewton.World_swigregister(WorldPtr)

class Material(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Material, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Material, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Material instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_setmethods__["m_material"] = _pynewton.Material_m_material_set
    __swig_getmethods__["m_material"] = _pynewton.Material_m_material_get
    if _newclass:m_material = property(_pynewton.Material_m_material_get, _pynewton.Material_m_material_set)
    def __init__(self, *args):
        """
        __init__(self, NewtonMaterial m) -> Material

        1
        """
        _swig_setattr(self, Material, 'this', _pynewton.new_Material(*args))
        _swig_setattr(self, Material, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Material):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def DisableContact(*args):
        """
        DisableContact(self)

        1
        """
        return _pynewton.Material_DisableContact(*args)

    def GetCurrentTimestep(*args):
        """
        GetCurrentTimestep(self) -> dFloat

        1
        """
        return _pynewton.Material_GetCurrentTimestep(*args)

    def GetContactFaceAttribute(*args):
        """
        GetContactFaceAttribute(self) -> unsigned int

        1
        """
        return _pynewton.Material_GetContactFaceAttribute(*args)

    def GetBodyCollisionID(*args):
        """
        GetBodyCollisionID(self, Body body) -> unsigned int

        1
        """
        return _pynewton.Material_GetBodyCollisionID(*args)

    def GetContactNormalSpeed(*args):
        """
        GetContactNormalSpeed(self, NewtonContact contactlHandle) -> dFloat

        1
        """
        return _pynewton.Material_GetContactNormalSpeed(*args)

    def GetContactForce(*args):
        """
        GetContactForce(self) -> PyObject

        1
        """
        return _pynewton.Material_GetContactForce(*args)

    def GetContactPosition(*args):
        """
        GetContactPosition(self) -> PyObject

        1
        """
        return _pynewton.Material_GetContactPosition(*args)

    def GetContactNormal(*args):
        """
        GetContactNormal(self) -> PyObject

        1
        """
        return _pynewton.Material_GetContactNormal(*args)

    def GetContactTangentDirections(*args):
        """
        GetContactTangentDirections(self) -> PyObject

        1
        """
        return _pynewton.Material_GetContactTangentDirections(*args)

    def GetContactTangentSpeed(*args):
        """
        GetContactTangentSpeed(self, NewtonContact contactlHandle, int index) -> dFloat

        1
        """
        return _pynewton.Material_GetContactTangentSpeed(*args)

    def SetContactSoftness(*args):
        """
        SetContactSoftness(self, dFloat softness)

        1
        """
        return _pynewton.Material_SetContactSoftness(*args)

    def SetContactElasticity(*args):
        """
        SetContactElasticity(self, dFloat restitution)

        1
        """
        return _pynewton.Material_SetContactElasticity(*args)

    def SetContactFrictionState(*args):
        """
        SetContactFrictionState(self, int state, int index)

        1
        """
        return _pynewton.Material_SetContactFrictionState(*args)

    def SetContactStaticFrictionCoef(*args):
        """
        SetContactStaticFrictionCoef(self, dFloat coef, int index)

        1
        """
        return _pynewton.Material_SetContactStaticFrictionCoef(*args)

    def SetContactKineticFrictionCoef(*args):
        """
        SetContactKineticFrictionCoef(self, dFloat coef, int index)

        1
        """
        return _pynewton.Material_SetContactKineticFrictionCoef(*args)

    def SetContactNormalAcceleration(*args):
        """
        SetContactNormalAcceleration(self, dFloat accel)

        1
        """
        return _pynewton.Material_SetContactNormalAcceleration(*args)

    def SetContactNormalDirection(*args):
        """
        SetContactNormalDirection(self, dFloat directionVector)

        1
        """
        return _pynewton.Material_SetContactNormalDirection(*args)

    def SetContactTangentAcceleration(*args):
        """
        SetContactTangentAcceleration(self, dFloat accel, int index)

        1
        """
        return _pynewton.Material_SetContactTangentAcceleration(*args)

    def ContactRotateTangentDirections(*args):
        """
        ContactRotateTangentDirections(self, dFloat directionVector)

        1
        """
        return _pynewton.Material_ContactRotateTangentDirections(*args)


class MaterialPtr(Material):
    def __init__(self, this):
        _swig_setattr(self, Material, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Material, 'thisown', 0)
        self.__class__ = Material
_pynewton.Material_swigregister(MaterialPtr)

class RayCollision(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, RayCollision, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, RayCollision, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ RayCollision instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_setmethods__["distance"] = _pynewton.RayCollision_distance_set
    __swig_getmethods__["distance"] = _pynewton.RayCollision_distance_get
    if _newclass:distance = property(_pynewton.RayCollision_distance_get, _pynewton.RayCollision_distance_set)
    __swig_setmethods__["normalX"] = _pynewton.RayCollision_normalX_set
    __swig_getmethods__["normalX"] = _pynewton.RayCollision_normalX_get
    if _newclass:normalX = property(_pynewton.RayCollision_normalX_get, _pynewton.RayCollision_normalX_set)
    __swig_setmethods__["normalY"] = _pynewton.RayCollision_normalY_set
    __swig_getmethods__["normalY"] = _pynewton.RayCollision_normalY_get
    if _newclass:normalY = property(_pynewton.RayCollision_normalY_get, _pynewton.RayCollision_normalY_set)
    __swig_setmethods__["normalZ"] = _pynewton.RayCollision_normalZ_set
    __swig_getmethods__["normalZ"] = _pynewton.RayCollision_normalZ_get
    if _newclass:normalZ = property(_pynewton.RayCollision_normalZ_get, _pynewton.RayCollision_normalZ_set)
    __swig_setmethods__["attributeID"] = _pynewton.RayCollision_attributeID_set
    __swig_getmethods__["attributeID"] = _pynewton.RayCollision_attributeID_get
    if _newclass:attributeID = property(_pynewton.RayCollision_attributeID_get, _pynewton.RayCollision_attributeID_set)
    def __init__(self, *args):
        """
        __init__(self) -> RayCollision

        1
        """
        _swig_setattr(self, RayCollision, 'this', _pynewton.new_RayCollision(*args))
        _swig_setattr(self, RayCollision, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_RayCollision):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class RayCollisionPtr(RayCollision):
    def __init__(self, this):
        _swig_setattr(self, RayCollision, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, RayCollision, 'thisown', 0)
        self.__class__ = RayCollision
_pynewton.RayCollision_swigregister(RayCollisionPtr)

class CollisionGeometry(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, CollisionGeometry, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, CollisionGeometry, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ CollisionGeometry instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> CollisionGeometry

        1
        """
        _swig_setattr(self, CollisionGeometry, 'this', _pynewton.new_CollisionGeometry(*args))
        _swig_setattr(self, CollisionGeometry, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_CollisionGeometry):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetNewtonCollision(*args):
        """
        GetNewtonCollision(self) -> NewtonCollision

        1
        """
        return _pynewton.CollisionGeometry_GetNewtonCollision(*args)

    def GetNewtonWorld(*args):
        """
        GetNewtonWorld(self) -> NewtonWorld

        1
        """
        return _pynewton.CollisionGeometry_GetNewtonWorld(*args)

    def PointDistance(*args):
        """
        PointDistance(self, dFloat point, dFloat matrix) -> PyObject

        1
        """
        return _pynewton.CollisionGeometry_PointDistance(*args)

    def ClosestPoint(*args):
        """
        ClosestPoint(self, dFloat matrixA, CollisionGeometry collsionB, dFloat matrixB) -> PyObject

        1
        """
        return _pynewton.CollisionGeometry_ClosestPoint(*args)

    def CalculateAABB(*args):
        """
        CalculateAABB(self, dFloat matrix) -> PyObject

        1
        """
        return _pynewton.CollisionGeometry_CalculateAABB(*args)

    def RayCast(*args):
        """
        RayCast(self, dFloat vectorA, dFloat vectorB) -> RayCollision

        1
        """
        return _pynewton.CollisionGeometry_RayCast(*args)


class CollisionGeometryPtr(CollisionGeometry):
    def __init__(self, this):
        _swig_setattr(self, CollisionGeometry, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CollisionGeometry, 'thisown', 0)
        self.__class__ = CollisionGeometry
_pynewton.CollisionGeometry_swigregister(CollisionGeometryPtr)

class CompoundCollision(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, CompoundCollision, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, CompoundCollision, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ CompoundCollision instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world) -> CompoundCollision

        1
        """
        _swig_setattr(self, CompoundCollision, 'this', _pynewton.new_CompoundCollision(*args))
        _swig_setattr(self, CompoundCollision, 'thisown', 1)
    def AddCollision(*args):
        """
        AddCollision(self, CollisionGeometry c)

        1
        """
        return _pynewton.CompoundCollision_AddCollision(*args)

    def Finalize(*args):
        """
        Finalize(self)

        1
        """
        return _pynewton.CompoundCollision_Finalize(*args)

    def __del__(self, destroy=_pynewton.delete_CompoundCollision):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class CompoundCollisionPtr(CompoundCollision):
    def __init__(self, this):
        _swig_setattr(self, CompoundCollision, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CompoundCollision, 'thisown', 0)
        self.__class__ = CompoundCollision
_pynewton.CompoundCollision_swigregister(CompoundCollisionPtr)

class Sphere(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Sphere, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Sphere, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Sphere instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat radiusX, dFloat radiusY, dFloat radiusZ, 
            dFloat offsetMatrix) -> Sphere

        1
        """
        _swig_setattr(self, Sphere, 'this', _pynewton.new_Sphere(*args))
        _swig_setattr(self, Sphere, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Sphere):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class SpherePtr(Sphere):
    def __init__(self, this):
        _swig_setattr(self, Sphere, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Sphere, 'thisown', 0)
        self.__class__ = Sphere
_pynewton.Sphere_swigregister(SpherePtr)

class Box(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Box, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Box, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Box instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat x, dFloat y, dFloat z, dFloat offsetMatrix) -> Box

        1
        """
        _swig_setattr(self, Box, 'this', _pynewton.new_Box(*args))
        _swig_setattr(self, Box, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Box):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class BoxPtr(Box):
    def __init__(self, this):
        _swig_setattr(self, Box, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Box, 'thisown', 0)
        self.__class__ = Box
_pynewton.Box_swigregister(BoxPtr)

class Cone(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Cone, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Cone, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Cone instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat radius, dFloat height, dFloat offsetMatrix) -> Cone

        1
        """
        _swig_setattr(self, Cone, 'this', _pynewton.new_Cone(*args))
        _swig_setattr(self, Cone, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Cone):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class ConePtr(Cone):
    def __init__(self, this):
        _swig_setattr(self, Cone, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Cone, 'thisown', 0)
        self.__class__ = Cone
_pynewton.Cone_swigregister(ConePtr)

class Capsule(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Capsule, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Capsule, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Capsule instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat radius, dFloat height, dFloat offsetMatrix) -> Capsule

        1
        """
        _swig_setattr(self, Capsule, 'this', _pynewton.new_Capsule(*args))
        _swig_setattr(self, Capsule, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Capsule):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class CapsulePtr(Capsule):
    def __init__(self, this):
        _swig_setattr(self, Capsule, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Capsule, 'thisown', 0)
        self.__class__ = Capsule
_pynewton.Capsule_swigregister(CapsulePtr)

class Cylinder(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Cylinder, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Cylinder, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Cylinder instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat radius, dFloat height, dFloat offsetMatrix) -> Cylinder

        1
        """
        _swig_setattr(self, Cylinder, 'this', _pynewton.new_Cylinder(*args))
        _swig_setattr(self, Cylinder, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Cylinder):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class CylinderPtr(Cylinder):
    def __init__(self, this):
        _swig_setattr(self, Cylinder, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Cylinder, 'thisown', 0)
        self.__class__ = Cylinder
_pynewton.Cylinder_swigregister(CylinderPtr)

class ChamferCylinder(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ChamferCylinder, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ChamferCylinder, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ ChamferCylinder instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat radius, dFloat height, dFloat offsetMatrix) -> ChamferCylinder

        1
        """
        _swig_setattr(self, ChamferCylinder, 'this', _pynewton.new_ChamferCylinder(*args))
        _swig_setattr(self, ChamferCylinder, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_ChamferCylinder):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class ChamferCylinderPtr(ChamferCylinder):
    def __init__(self, this):
        _swig_setattr(self, ChamferCylinder, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ChamferCylinder, 'thisown', 0)
        self.__class__ = ChamferCylinder
_pynewton.ChamferCylinder_swigregister(ChamferCylinderPtr)

class ConvexHull(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ConvexHull, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ConvexHull, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ ConvexHull instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, PyObject pylist, dFloat offsetMatrix) -> ConvexHull

        1
        """
        _swig_setattr(self, ConvexHull, 'this', _pynewton.new_ConvexHull(*args))
        _swig_setattr(self, ConvexHull, 'thisown', 1)
    def SetUserID(*args):
        """
        SetUserID(self, unsigned int id)

        1
        """
        return _pynewton.ConvexHull_SetUserID(*args)

    def GetUserID(*args):
        """
        GetUserID(self) -> unsigned int

        1
        """
        return _pynewton.ConvexHull_GetUserID(*args)

    def CalculateVolume(*args):
        """
        CalculateVolume(self) -> dFloat

        1
        """
        return _pynewton.ConvexHull_CalculateVolume(*args)

    def CalculateInertiaMatrix(*args):
        """
        CalculateInertiaMatrix(self) -> PyObject

        1
        """
        return _pynewton.ConvexHull_CalculateInertiaMatrix(*args)

    def __del__(self, destroy=_pynewton.delete_ConvexHull):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class ConvexHullPtr(ConvexHull):
    def __init__(self, this):
        _swig_setattr(self, ConvexHull, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ConvexHull, 'thisown', 0)
        self.__class__ = ConvexHull
_pynewton.ConvexHull_swigregister(ConvexHullPtr)

class ConvexHullModifier(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ConvexHullModifier, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ConvexHullModifier, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ ConvexHullModifier instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, ConvexHull hull) -> ConvexHullModifier

        1
        """
        _swig_setattr(self, ConvexHullModifier, 'this', _pynewton.new_ConvexHullModifier(*args))
        _swig_setattr(self, ConvexHullModifier, 'thisown', 1)
    def SetMatrix(*args):
        """
        SetMatrix(self, dFloat matrix)

        1
        """
        return _pynewton.ConvexHullModifier_SetMatrix(*args)

    def GetMatrix(*args):
        """
        GetMatrix(self) -> PyObject

        1
        """
        return _pynewton.ConvexHullModifier_GetMatrix(*args)

    def __del__(self, destroy=_pynewton.delete_ConvexHullModifier):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class ConvexHullModifierPtr(ConvexHullModifier):
    def __init__(self, this):
        _swig_setattr(self, ConvexHullModifier, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ConvexHullModifier, 'thisown', 0)
        self.__class__ = ConvexHullModifier
_pynewton.ConvexHullModifier_swigregister(ConvexHullModifierPtr)

class NullCollider(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NullCollider, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NullCollider, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ NullCollider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world) -> NullCollider

        1
        """
        _swig_setattr(self, NullCollider, 'this', _pynewton.new_NullCollider(*args))
        _swig_setattr(self, NullCollider, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_NullCollider):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class NullColliderPtr(NullCollider):
    def __init__(self, this):
        _swig_setattr(self, NullCollider, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NullCollider, 'thisown', 0)
        self.__class__ = NullCollider
_pynewton.NullCollider_swigregister(NullColliderPtr)

class TreeCollisionUserCallback(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TreeCollisionUserCallback, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TreeCollisionUserCallback, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ TreeCollisionUserCallback instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def OnCallback(*args):
        """
        OnCallback(self, Body bodyWithTreeCollision, Body body, dFloat vertex, 
            int vertexstrideInBytes, int indexCount, int indexArray)

        1
        """
        return _pynewton.TreeCollisionUserCallback_OnCallback(*args)

    def __init__(self, *args):
        """
        __init__(self) -> TreeCollisionUserCallback

        1
        """
        if self.__class__ == TreeCollisionUserCallback:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, TreeCollisionUserCallback, 'this', _pynewton.new_TreeCollisionUserCallback(*args))
        _swig_setattr(self, TreeCollisionUserCallback, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_TreeCollisionUserCallback):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_TreeCollisionUserCallback(self)
        return weakref_proxy(self)

class TreeCollisionUserCallbackPtr(TreeCollisionUserCallback):
    def __init__(self, this):
        _swig_setattr(self, TreeCollisionUserCallback, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TreeCollisionUserCallback, 'thisown', 0)
        self.__class__ = TreeCollisionUserCallback
_pynewton.TreeCollisionUserCallback_swigregister(TreeCollisionUserCallbackPtr)

class TreeCollision(CollisionGeometry):
    """1"""
    __swig_setmethods__ = {}
    for _s in [CollisionGeometry]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, TreeCollision, name, value)
    __swig_getmethods__ = {}
    for _s in [CollisionGeometry]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, TreeCollision, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ TreeCollision instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world) -> TreeCollision

        1
        """
        _swig_setattr(self, TreeCollision, 'this', _pynewton.new_TreeCollision(*args))
        _swig_setattr(self, TreeCollision, 'thisown', 1)
    def BeginBuild(*args):
        """
        BeginBuild(self)

        1
        """
        return _pynewton.TreeCollision_BeginBuild(*args)

    def AddFace(*args):
        """
        AddFace(self, PyObject vertexList, int faceAttribute)

        1
        """
        return _pynewton.TreeCollision_AddFace(*args)

    def EndBuild(*args):
        """
        EndBuild(self, int optimize)

        1
        """
        return _pynewton.TreeCollision_EndBuild(*args)

    def GetFaceAttribute(*args):
        """
        GetFaceAttribute(self, int faceIndexArray) -> int

        1
        """
        return _pynewton.TreeCollision_GetFaceAttribute(*args)

    def SetFaceAttribute(*args):
        """
        SetFaceAttribute(self, int faceIndexArray, int attribute)

        1
        """
        return _pynewton.TreeCollision_SetFaceAttribute(*args)

    def Serialize(*args):
        """
        Serialize(self, char filename) -> bool

        1
        """
        return _pynewton.TreeCollision_Serialize(*args)

    def Deserialize(*args):
        """
        Deserialize(self, char filename) -> bool

        1
        """
        return _pynewton.TreeCollision_Deserialize(*args)

    def __del__(self, destroy=_pynewton.delete_TreeCollision):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class TreeCollisionPtr(TreeCollision):
    def __init__(self, this):
        _swig_setattr(self, TreeCollision, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TreeCollision, 'thisown', 0)
        self.__class__ = TreeCollision
_pynewton.TreeCollision_swigregister(TreeCollisionPtr)

class Body(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Body, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Body, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Body instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_setmethods__["m_body"] = _pynewton.Body_m_body_set
    __swig_getmethods__["m_body"] = _pynewton.Body_m_body_get
    if _newclass:m_body = property(_pynewton.Body_m_body_get, _pynewton.Body_m_body_set)
    __swig_setmethods__["m_world"] = _pynewton.Body_m_world_set
    __swig_getmethods__["m_world"] = _pynewton.Body_m_world_get
    if _newclass:m_world = property(_pynewton.Body_m_world_get, _pynewton.Body_m_world_set)
    __swig_setmethods__["m_collision"] = _pynewton.Body_m_collision_set
    __swig_getmethods__["m_collision"] = _pynewton.Body_m_collision_get
    if _newclass:m_collision = property(_pynewton.Body_m_collision_get, _pynewton.Body_m_collision_set)
    __swig_setmethods__["userData"] = _pynewton.Body_userData_set
    __swig_getmethods__["userData"] = _pynewton.Body_userData_get
    if _newclass:userData = property(_pynewton.Body_userData_get, _pynewton.Body_userData_set)
    __swig_setmethods__["m_cleanupOnDestruct"] = _pynewton.Body_m_cleanupOnDestruct_set
    __swig_getmethods__["m_cleanupOnDestruct"] = _pynewton.Body_m_cleanupOnDestruct_get
    if _newclass:m_cleanupOnDestruct = property(_pynewton.Body_m_cleanupOnDestruct_get, _pynewton.Body_m_cleanupOnDestruct_set)
    __swig_setmethods__["m_zombie"] = _pynewton.Body_m_zombie_set
    __swig_getmethods__["m_zombie"] = _pynewton.Body_m_zombie_get
    if _newclass:m_zombie = property(_pynewton.Body_m_zombie_get, _pynewton.Body_m_zombie_set)
    def __init__(self, *args):
        """
        __init__(self, NewtonBody body) -> Body
        __init__(self, World w, CollisionGeometry g) -> Body
        __init__(self, Body body) -> Body

        1
        """
        if self.__class__ == Body:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Body, 'this', _pynewton.new_Body(*args))
        _swig_setattr(self, Body, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Body):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetNewtonCollision(*args):
        """
        GetNewtonCollision(self) -> NewtonCollision

        1
        """
        return _pynewton.Body_GetNewtonCollision(*args)

    def GetNewtonWorld(*args):
        """
        GetNewtonWorld(self) -> NewtonWorld

        1
        """
        return _pynewton.Body_GetNewtonWorld(*args)

    def IDKey(*args):
        """
        IDKey(self) -> int

        1
        """
        return _pynewton.Body_IDKey(*args)

    def GetWorld(*args):
        """
        GetWorld(self) -> World

        1
        """
        return _pynewton.Body_GetWorld(*args)

    def SetMassMatrix(*args):
        """
        SetMassMatrix(self, dFloat mass, dFloat ix, dFloat iy, dFloat iz)

        1
        """
        return _pynewton.Body_SetMassMatrix(*args)

    def SetMatrix(*args):
        """
        SetMatrix(self, dFloat matrix)

        1
        """
        return _pynewton.Body_SetMatrix(*args)

    def SetMatrixRecursive(*args):
        """
        SetMatrixRecursive(self, dFloat matrix)

        1
        """
        return _pynewton.Body_SetMatrixRecursive(*args)

    def GetMatrix(*args):
        """
        GetMatrix(self) -> PyObject

        1
        """
        return _pynewton.Body_GetMatrix(*args)

    def GetMassMatrix(*args):
        """
        GetMassMatrix(self) -> PyObject

        1
        """
        return _pynewton.Body_GetMassMatrix(*args)

    def SetOmega(*args):
        """
        SetOmega(self, dFloat omega)

        1
        """
        return _pynewton.Body_SetOmega(*args)

    def SetForce(*args):
        """
        SetForce(self, dFloat force)

        1
        """
        return _pynewton.Body_SetForce(*args)

    def SetTorque(*args):
        """
        SetTorque(self, dFloat torque)

        1
        """
        return _pynewton.Body_SetTorque(*args)

    def AddForce(*args):
        """
        AddForce(self, dFloat force)

        1
        """
        return _pynewton.Body_AddForce(*args)

    def AddTorque(*args):
        """
        AddTorque(self, dFloat torque)

        1
        """
        return _pynewton.Body_AddTorque(*args)

    def GetForce(*args):
        """
        GetForce(self) -> PyObject

        1
        """
        return _pynewton.Body_GetForce(*args)

    def GetOmega(*args):
        """
        GetOmega(self) -> PyObject

        1
        """
        return _pynewton.Body_GetOmega(*args)

    def GetVelocity(*args):
        """
        GetVelocity(self) -> PyObject

        1
        """
        return _pynewton.Body_GetVelocity(*args)

    def GetTorque(*args):
        """
        GetTorque(self) -> PyObject

        1
        """
        return _pynewton.Body_GetTorque(*args)

    def GetCentreOfMass(*args):
        """
        GetCentreOfMass(self) -> PyObject

        1
        """
        return _pynewton.Body_GetCentreOfMass(*args)

    def SetVelocity(*args):
        """
        SetVelocity(self, dFloat velocity)

        1
        """
        return _pynewton.Body_SetVelocity(*args)

    def Freeze(*args):
        """
        Freeze(self)

        1
        """
        return _pynewton.Body_Freeze(*args)

    def Unfreeze(*args):
        """
        Unfreeze(self)

        1
        """
        return _pynewton.Body_Unfreeze(*args)

    def SetCentreOfMass(*args):
        """
        SetCentreOfMass(self, dFloat vector)

        1
        """
        return _pynewton.Body_SetCentreOfMass(*args)

    def SetLinearDamping(*args):
        """
        SetLinearDamping(self, dFloat linearDamp)

        1
        """
        return _pynewton.Body_SetLinearDamping(*args)

    def SetAngularDamping(*args):
        """
        SetAngularDamping(self, dFloat vector)

        1
        """
        return _pynewton.Body_SetAngularDamping(*args)

    def CoriolisForcesMode(*args):
        """
        CoriolisForcesMode(self, int mode)

        1
        """
        return _pynewton.Body_CoriolisForcesMode(*args)

    def SetCollision(*args):
        """
        SetCollision(self, CollisionGeometry collision)

        1
        """
        return _pynewton.Body_SetCollision(*args)

    def SetAutoFreeze(*args):
        """
        SetAutoFreeze(self, int state)

        1
        """
        return _pynewton.Body_SetAutoFreeze(*args)

    def SetFreezeTreshold(*args):
        """
        SetFreezeTreshold(self, dFloat freezeSpeed2, dFloat freezeOmega2, int framesCount)

        1
        """
        return _pynewton.Body_SetFreezeTreshold(*args)

    def GetSleepingState(*args):
        """
        GetSleepingState(self) -> int

        1
        """
        return _pynewton.Body_GetSleepingState(*args)

    def GetAutoFreeze(*args):
        """
        GetAutoFreeze(self) -> int

        1
        """
        return _pynewton.Body_GetAutoFreeze(*args)

    def GetLinearDamping(*args):
        """
        GetLinearDamping(self) -> dFloat

        1
        """
        return _pynewton.Body_GetLinearDamping(*args)

    def GetAngularDamping(*args):
        """
        GetAngularDamping(self) -> PyObject

        1
        """
        return _pynewton.Body_GetAngularDamping(*args)

    def GetAABB(*args):
        """
        GetAABB(self) -> PyObject

        1
        """
        return _pynewton.Body_GetAABB(*args)

    def GetFreezeTreshold(*args):
        """
        GetFreezeTreshold(self) -> PyObject

        1
        """
        return _pynewton.Body_GetFreezeTreshold(*args)

    def SetMaterialGroupID(*args):
        """
        SetMaterialGroupID(self, int id)

        1
        """
        return _pynewton.Body_SetMaterialGroupID(*args)

    def SetContinuousCollisionMode(*args):
        """
        SetContinuousCollisionMode(self, unsigned int state)

        1
        """
        return _pynewton.Body_SetContinuousCollisionMode(*args)

    def SetJointRecursiveCollision(*args):
        """
        SetJointRecursiveCollision(self, unsigned int state)

        1
        """
        return _pynewton.Body_SetJointRecursiveCollision(*args)

    def SetDestructorCallback(*args):
        """
        SetDestructorCallback(self, NewtonBodyDestructor callback)

        1
        """
        return _pynewton.Body_SetDestructorCallback(*args)

    def SetUserData(*args):
        """
        SetUserData(self, PyObject data)

        1
        """
        return _pynewton.Body_SetUserData(*args)

    def GetUserData(*args):
        """
        GetUserData(self) -> PyObject

        1
        """
        return _pynewton.Body_GetUserData(*args)

    def MaterialGroupID(*args):
        """
        MaterialGroupID(self) -> int

        1
        """
        return _pynewton.Body_MaterialGroupID(*args)

    def GetContinuousCollisionMode(*args):
        """
        GetContinuousCollisionMode(self) -> int

        1
        """
        return _pynewton.Body_GetContinuousCollisionMode(*args)

    def GetJointRecursiveCollision(*args):
        """
        GetJointRecursiveCollision(self) -> int

        1
        """
        return _pynewton.Body_GetJointRecursiveCollision(*args)

    def GetInvMass(*args):
        """
        GetInvMass(self) -> PyObject

        1
        """
        return _pynewton.Body_GetInvMass(*args)

    def AddBuoyancyForce(*args):
        """
        AddBuoyancyForce(self, dFloat fluidDensity, dFloat fluidLinearViscosity, dFloat fluidAngularViscosity, 
            dFloat gravityVector, 
            dFloat plane)

        1
        """
        return _pynewton.Body_AddBuoyancyForce(*args)

    def ForEachPolygonDo(*args):
        """
        ForEachPolygonDo(self, NewtonCollisionIterator callback)

        1
        """
        return _pynewton.Body_ForEachPolygonDo(*args)

    def AddBodyImpulse(*args):
        """
        AddBodyImpulse(self, dFloat velocity, dFloat point)

        1
        """
        return _pynewton.Body_AddBodyImpulse(*args)

    def OnTreeCollisionWith(*args):
        """
        OnTreeCollisionWith(self, Body body)

        1
        """
        return _pynewton.Body_OnTreeCollisionWith(*args)

    def OnApplyForceAndTorque(*args):
        """
        OnApplyForceAndTorque(self)

        1
        """
        return _pynewton.Body_OnApplyForceAndTorque(*args)

    def OnAutoactive(*args):
        """
        OnAutoactive(self, unsigned int state)

        1
        """
        return _pynewton.Body_OnAutoactive(*args)

    def OnTransform(*args):
        """
        OnTransform(self)

        1
        """
        return _pynewton.Body_OnTransform(*args)

    def OnDestruct(*args):
        """
        OnDestruct(self)

        1
        """
        return _pynewton.Body_OnDestruct(*args)

    def WrapperFor(*args):
        """
        WrapperFor(NewtonBody bodyptr) -> Body

        1
        """
        return _pynewton.Body_WrapperFor(*args)

    if _newclass:WrapperFor = staticmethod(WrapperFor)
    __swig_getmethods__["WrapperFor"] = lambda x: WrapperFor
    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_Body(self)
        return weakref_proxy(self)

class BodyPtr(Body):
    def __init__(self, this):
        _swig_setattr(self, Body, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Body, 'thisown', 0)
        self.__class__ = Body
_pynewton.Body_swigregister(BodyPtr)

def Body_WrapperFor(*args):
    """
    Body_WrapperFor(NewtonBody bodyptr) -> Body

    1
    """
    return _pynewton.Body_WrapperFor(*args)

class Joint(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Joint, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Joint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Joint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_setmethods__["m_world"] = _pynewton.Joint_m_world_set
    __swig_getmethods__["m_world"] = _pynewton.Joint_m_world_get
    if _newclass:m_world = property(_pynewton.Joint_m_world_get, _pynewton.Joint_m_world_set)
    __swig_setmethods__["m_joint"] = _pynewton.Joint_m_joint_set
    __swig_getmethods__["m_joint"] = _pynewton.Joint_m_joint_get
    if _newclass:m_joint = property(_pynewton.Joint_m_joint_get, _pynewton.Joint_m_joint_set)
    __swig_setmethods__["m_userData"] = _pynewton.Joint_m_userData_set
    __swig_getmethods__["m_userData"] = _pynewton.Joint_m_userData_get
    if _newclass:m_userData = property(_pynewton.Joint_m_userData_get, _pynewton.Joint_m_userData_set)
    def __init__(self, *args):
        """
        __init__(self) -> Joint

        1
        """
        _swig_setattr(self, Joint, 'this', _pynewton.new_Joint(*args))
        _swig_setattr(self, Joint, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Joint):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def ForceDestroy(*args):
        """
        ForceDestroy(self)

        1
        """
        return _pynewton.Joint_ForceDestroy(*args)

    def SetUserData(*args):
        """
        SetUserData(self, PyObject userData)

        1
        """
        return _pynewton.Joint_SetUserData(*args)

    def GetUserData(*args):
        """
        GetUserData(self) -> PyObject

        1
        """
        return _pynewton.Joint_GetUserData(*args)

    def SetCollisionState(*args):
        """
        SetCollisionState(self, int state)

        1
        """
        return _pynewton.Joint_SetCollisionState(*args)

    def GetCollisionState(*args):
        """
        GetCollisionState(self) -> int

        1
        """
        return _pynewton.Joint_GetCollisionState(*args)

    def SetStiffness(*args):
        """
        SetStiffness(self, dFloat stiffness)

        1
        """
        return _pynewton.Joint_SetStiffness(*args)

    def GetStiffness(*args):
        """
        GetStiffness(self) -> dFloat

        1
        """
        return _pynewton.Joint_GetStiffness(*args)

    def OnDestroyed(*args):
        """
        OnDestroyed(self)

        1
        """
        return _pynewton.Joint_OnDestroyed(*args)


class JointPtr(Joint):
    def __init__(self, this):
        _swig_setattr(self, Joint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Joint, 'thisown', 0)
        self.__class__ = Joint
_pynewton.Joint_swigregister(JointPtr)

class BallJoint(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BallJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BallJoint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ BallJoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pivotPoint, Body childBody, Body parentBody) -> BallJoint

        1
        """
        if self.__class__ == BallJoint:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, BallJoint, 'this', _pynewton.new_BallJoint(*args))
        _swig_setattr(self, BallJoint, 'thisown', 1)
    def GetJointAngle(*args):
        """
        GetJointAngle(self) -> PyObject

        1
        """
        return _pynewton.BallJoint_GetJointAngle(*args)

    def GetJointOmega(*args):
        """
        GetJointOmega(self) -> PyObject

        1
        """
        return _pynewton.BallJoint_GetJointOmega(*args)

    def GetJointForce(*args):
        """
        GetJointForce(self) -> PyObject

        1
        """
        return _pynewton.BallJoint_GetJointForce(*args)

    def OnCallback(*args):
        """
        OnCallback(self)

        1
        """
        return _pynewton.BallJoint_OnCallback(*args)

    def SetConeLimits(*args):
        """
        SetConeLimits(self, dFloat pin, dFloat maxConeAngle, dFloat maxTwistAngle)

        1
        """
        return _pynewton.BallJoint_SetConeLimits(*args)

    def __del__(self, destroy=_pynewton.delete_BallJoint):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_BallJoint(self)
        return weakref_proxy(self)

class BallJointPtr(BallJoint):
    def __init__(self, this):
        _swig_setattr(self, BallJoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BallJoint, 'thisown', 0)
        self.__class__ = BallJoint
_pynewton.BallJoint_swigregister(BallJointPtr)

class Hinge(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Hinge, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Hinge, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Hinge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pivotPoint, dFloat pinDir, Body childBody, 
            Body parentBody) -> Hinge

        1
        """
        if self.__class__ == Hinge:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Hinge, 'this', _pynewton.new_Hinge(*args))
        _swig_setattr(self, Hinge, 'thisown', 1)
    def GetJointAngle(*args):
        """
        GetJointAngle(self) -> dFloat

        1
        """
        return _pynewton.Hinge_GetJointAngle(*args)

    def GetJointOmega(*args):
        """
        GetJointOmega(self) -> dFloat

        1
        """
        return _pynewton.Hinge_GetJointOmega(*args)

    def GetJointForce(*args):
        """
        GetJointForce(self) -> PyObject

        1
        """
        return _pynewton.Hinge_GetJointForce(*args)

    def CalculateStopAlpha(*args):
        """
        CalculateStopAlpha(self, NewtonHingeSliderUpdateDesc desc, dFloat angle) -> dFloat

        1
        """
        return _pynewton.Hinge_CalculateStopAlpha(*args)

    def OnCallback(*args):
        """
        OnCallback(self, NewtonHingeSliderUpdateDesc desc) -> unsigned int

        1
        """
        return _pynewton.Hinge_OnCallback(*args)

    def __del__(self, destroy=_pynewton.delete_Hinge):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_Hinge(self)
        return weakref_proxy(self)

class HingePtr(Hinge):
    def __init__(self, this):
        _swig_setattr(self, Hinge, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Hinge, 'thisown', 0)
        self.__class__ = Hinge
_pynewton.Hinge_swigregister(HingePtr)

class Slider(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Slider, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Slider, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Slider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pivotPoint, dFloat pinDir, Body childBody, 
            Body parentBody) -> Slider

        1
        """
        if self.__class__ == Slider:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Slider, 'this', _pynewton.new_Slider(*args))
        _swig_setattr(self, Slider, 'thisown', 1)
    def GetPosition(*args):
        """
        GetPosition(self) -> dFloat

        1
        """
        return _pynewton.Slider_GetPosition(*args)

    def GetVelocity(*args):
        """
        GetVelocity(self) -> dFloat

        1
        """
        return _pynewton.Slider_GetVelocity(*args)

    def GetForce(*args):
        """
        GetForce(self) -> PyObject

        1
        """
        return _pynewton.Slider_GetForce(*args)

    def SetMinLimit(*args):
        """
        SetMinLimit(self, dFloat f)

        1
        """
        return _pynewton.Slider_SetMinLimit(*args)

    def SetMaxLimit(*args):
        """
        SetMaxLimit(self, dFloat f)

        1
        """
        return _pynewton.Slider_SetMaxLimit(*args)

    def GetMinLimit(*args):
        """
        GetMinLimit(self) -> dFloat

        1
        """
        return _pynewton.Slider_GetMinLimit(*args)

    def GetMaxLimit(*args):
        """
        GetMaxLimit(self) -> dFloat

        1
        """
        return _pynewton.Slider_GetMaxLimit(*args)

    def CalculateStopAccel(*args):
        """
        CalculateStopAccel(self, NewtonHingeSliderUpdateDesc desc, dFloat position) -> dFloat

        1
        """
        return _pynewton.Slider_CalculateStopAccel(*args)

    def OnCallback(*args):
        """
        OnCallback(self, NewtonHingeSliderUpdateDesc desc) -> unsigned int

        1
        """
        return _pynewton.Slider_OnCallback(*args)

    def __del__(self, destroy=_pynewton.delete_Slider):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_Slider(self)
        return weakref_proxy(self)

class SliderPtr(Slider):
    def __init__(self, this):
        _swig_setattr(self, Slider, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Slider, 'thisown', 0)
        self.__class__ = Slider
_pynewton.Slider_swigregister(SliderPtr)

class Corkscrew(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Corkscrew, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Corkscrew, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Corkscrew instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pivotPoint, dFloat pinDir, Body childBody, 
            Body parentBody) -> Corkscrew

        1
        """
        if self.__class__ == Corkscrew:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Corkscrew, 'this', _pynewton.new_Corkscrew(*args))
        _swig_setattr(self, Corkscrew, 'thisown', 1)
    def GetPosition(*args):
        """
        GetPosition(self) -> dFloat

        1
        """
        return _pynewton.Corkscrew_GetPosition(*args)

    def GetAngle(*args):
        """
        GetAngle(self) -> dFloat

        1
        """
        return _pynewton.Corkscrew_GetAngle(*args)

    def GetVelocity(*args):
        """
        GetVelocity(self) -> dFloat

        1
        """
        return _pynewton.Corkscrew_GetVelocity(*args)

    def GetOmega(*args):
        """
        GetOmega(self) -> dFloat

        1
        """
        return _pynewton.Corkscrew_GetOmega(*args)

    def GetForce(*args):
        """
        GetForce(self) -> PyObject

        1
        """
        return _pynewton.Corkscrew_GetForce(*args)

    def OnCallback(*args):
        """
        OnCallback(self, NewtonHingeSliderUpdateDesc desc) -> unsigned int

        1
        """
        return _pynewton.Corkscrew_OnCallback(*args)

    def CalculateStopAlpha(*args):
        """
        CalculateStopAlpha(self, NewtonHingeSliderUpdateDesc desc, dFloat angle) -> dFloat

        1
        """
        return _pynewton.Corkscrew_CalculateStopAlpha(*args)

    def CalculateStopAccel(*args):
        """
        CalculateStopAccel(self, NewtonJoint corkscrew, NewtonHingeSliderUpdateDesc desc, 
            dFloat position) -> dFloat

        1
        """
        return _pynewton.Corkscrew_CalculateStopAccel(*args)

    def __del__(self, destroy=_pynewton.delete_Corkscrew):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_Corkscrew(self)
        return weakref_proxy(self)

class CorkscrewPtr(Corkscrew):
    def __init__(self, this):
        _swig_setattr(self, Corkscrew, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Corkscrew, 'thisown', 0)
        self.__class__ = Corkscrew
_pynewton.Corkscrew_swigregister(CorkscrewPtr)

class UniversalJoint(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, UniversalJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, UniversalJoint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ UniversalJoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pivotPoint, dFloat pinDir0, dFloat pinDir1, 
            Body childBody, Body parentBody) -> UniversalJoint

        1
        """
        if self.__class__ == UniversalJoint:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, UniversalJoint, 'this', _pynewton.new_UniversalJoint(*args))
        _swig_setattr(self, UniversalJoint, 'thisown', 1)
    def GetAngle0(*args):
        """
        GetAngle0(self) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_GetAngle0(*args)

    def GetAngle1(*args):
        """
        GetAngle1(self) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_GetAngle1(*args)

    def GetOmega0(*args):
        """
        GetOmega0(self) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_GetOmega0(*args)

    def GetOmega1(*args):
        """
        GetOmega1(self) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_GetOmega1(*args)

    def GetForce(*args):
        """
        GetForce(self) -> PyObject

        1
        """
        return _pynewton.UniversalJoint_GetForce(*args)

    def OnCallback(*args):
        """
        OnCallback(self, NewtonHingeSliderUpdateDesc desc) -> unsigned int

        1
        """
        return _pynewton.UniversalJoint_OnCallback(*args)

    def CalculateStopAlpha0(*args):
        """
        CalculateStopAlpha0(self, NewtonHingeSliderUpdateDesc desc, dFloat angle) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_CalculateStopAlpha0(*args)

    def CalculateStopAlpha1(*args):
        """
        CalculateStopAlpha1(self, NewtonHingeSliderUpdateDesc desc, dFloat angle) -> dFloat

        1
        """
        return _pynewton.UniversalJoint_CalculateStopAlpha1(*args)

    def __del__(self, destroy=_pynewton.delete_UniversalJoint):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_UniversalJoint(self)
        return weakref_proxy(self)

class UniversalJointPtr(UniversalJoint):
    def __init__(self, this):
        _swig_setattr(self, UniversalJoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UniversalJoint, 'thisown', 0)
        self.__class__ = UniversalJoint
_pynewton.UniversalJoint_swigregister(UniversalJointPtr)

class UpVector(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, UpVector, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, UpVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ UpVector instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat pinDir, Body body) -> UpVector

        1
        """
        if self.__class__ == UpVector:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, UpVector, 'this', _pynewton.new_UpVector(*args))
        _swig_setattr(self, UpVector, 'thisown', 1)
    def GetPin(*args):
        """
        GetPin(self) -> PyObject

        1
        """
        return _pynewton.UpVector_GetPin(*args)

    def SetPin(*args):
        """
        SetPin(self, dFloat pin)

        1
        """
        return _pynewton.UpVector_SetPin(*args)

    def __del__(self, destroy=_pynewton.delete_UpVector):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_UpVector(self)
        return weakref_proxy(self)

class UpVectorPtr(UpVector):
    def __init__(self, this):
        _swig_setattr(self, UpVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UpVector, 'thisown', 0)
        self.__class__ = UpVector
_pynewton.UpVector_swigregister(UpVectorPtr)

class UserJoint(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, UserJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, UserJoint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ UserJoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> UserJoint

        1
        """
        _swig_setattr(self, UserJoint, 'this', _pynewton.new_UserJoint(*args))
        _swig_setattr(self, UserJoint, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_UserJoint):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass


class UserJointPtr(UserJoint):
    def __init__(self, this):
        _swig_setattr(self, UserJoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UserJoint, 'thisown', 0)
        self.__class__ = UserJoint
_pynewton.UserJoint_swigregister(UserJointPtr)

class Tire(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Tire, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Tire, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Tire instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_setmethods__["m_vehicle"] = _pynewton.Tire_m_vehicle_set
    __swig_getmethods__["m_vehicle"] = _pynewton.Tire_m_vehicle_get
    if _newclass:m_vehicle = property(_pynewton.Tire_m_vehicle_get, _pynewton.Tire_m_vehicle_set)
    __swig_setmethods__["m_tireID"] = _pynewton.Tire_m_tireID_set
    __swig_getmethods__["m_tireID"] = _pynewton.Tire_m_tireID_get
    if _newclass:m_tireID = property(_pynewton.Tire_m_tireID_get, _pynewton.Tire_m_tireID_set)
    __swig_setmethods__["m_userData"] = _pynewton.Tire_m_userData_set
    __swig_getmethods__["m_userData"] = _pynewton.Tire_m_userData_get
    if _newclass:m_userData = property(_pynewton.Tire_m_userData_get, _pynewton.Tire_m_userData_set)
    def __init__(self, *args):
        """
        __init__(self, Vehicle v, void tireid) -> Tire
        __init__(self, Tire t) -> Tire

        1
        """
        _swig_setattr(self, Tire, 'this', _pynewton.new_Tire(*args))
        _swig_setattr(self, Tire, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Tire):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetUserData(*args):
        """
        SetUserData(self, PyObject userData)

        1
        """
        return _pynewton.Tire_SetUserData(*args)

    def GetUserData(*args):
        """
        GetUserData(self) -> PyObject

        1
        """
        return _pynewton.Tire_GetUserData(*args)

    def IsAirBorne(*args):
        """
        IsAirBorne(self) -> bool

        1
        """
        return _pynewton.Tire_IsAirBorne(*args)

    def LostSideGrip(*args):
        """
        LostSideGrip(self) -> bool

        1
        """
        return _pynewton.Tire_LostSideGrip(*args)

    def LostTraction(*args):
        """
        LostTraction(self) -> bool

        1
        """
        return _pynewton.Tire_LostTraction(*args)

    def GetOmega(*args):
        """
        GetOmega(self) -> dFloat

        1
        """
        return _pynewton.Tire_GetOmega(*args)

    def GetNormalLoad(*args):
        """
        GetNormalLoad(self) -> dFloat

        1
        """
        return _pynewton.Tire_GetNormalLoad(*args)

    def GetSteerAngle(*args):
        """
        GetSteerAngle(self) -> dFloat

        1
        """
        return _pynewton.Tire_GetSteerAngle(*args)

    def GetLateralSpeed(*args):
        """
        GetLateralSpeed(self) -> dFloat

        1
        """
        return _pynewton.Tire_GetLateralSpeed(*args)

    def GetLongitudinalSpeed(*args):
        """
        GetLongitudinalSpeed(self) -> dFloat

        1
        """
        return _pynewton.Tire_GetLongitudinalSpeed(*args)

    def GetMatrix(*args):
        """
        GetMatrix(self) -> PyObject

        1
        """
        return _pynewton.Tire_GetMatrix(*args)

    def SetTorque(*args):
        """
        SetTorque(self, dFloat torque)

        1
        """
        return _pynewton.Tire_SetTorque(*args)

    def SetSteerAngle(*args):
        """
        SetSteerAngle(self, dFloat angle)

        1
        """
        return _pynewton.Tire_SetSteerAngle(*args)

    def SetMaxSideSleepSpeed(*args):
        """
        SetMaxSideSleepSpeed(self, dFloat speed)

        1
        """
        return _pynewton.Tire_SetMaxSideSleepSpeed(*args)

    def SetSideSleepCoeficient(*args):
        """
        SetSideSleepCoeficient(self, dFloat coeficient)

        1
        """
        return _pynewton.Tire_SetSideSleepCoeficient(*args)

    def SetMaxLongitudinalSlideSpeed(*args):
        """
        SetMaxLongitudinalSlideSpeed(self, dFloat speed)

        1
        """
        return _pynewton.Tire_SetMaxLongitudinalSlideSpeed(*args)

    def SetLongitudinalSlideCoeficient(*args):
        """
        SetLongitudinalSlideCoeficient(self, dFloat coeficient)

        1
        """
        return _pynewton.Tire_SetLongitudinalSlideCoeficient(*args)

    def CalculateMaxBrakeAcceleration(*args):
        """
        CalculateMaxBrakeAcceleration(self) -> dFloat

        1
        """
        return _pynewton.Tire_CalculateMaxBrakeAcceleration(*args)

    def SetBrakeAcceleration(*args):
        """
        SetBrakeAcceleration(self, dFloat accel, dFloat torqueLimit)

        1
        """
        return _pynewton.Tire_SetBrakeAcceleration(*args)


class TirePtr(Tire):
    def __init__(self, this):
        _swig_setattr(self, Tire, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Tire, 'thisown', 0)
        self.__class__ = Tire
_pynewton.Tire_swigregister(TirePtr)

class Vehicle(Joint):
    """1"""
    __swig_setmethods__ = {}
    for _s in [Joint]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vehicle, name, value)
    __swig_getmethods__ = {}
    for _s in [Joint]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Vehicle, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ Vehicle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, dFloat upDir, Body body) -> Vehicle

        1
        """
        if self.__class__ == Vehicle:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Vehicle, 'this', _pynewton.new_Vehicle(*args))
        _swig_setattr(self, Vehicle, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_Vehicle):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def Reset(*args):
        """
        Reset(self)

        1
        """
        return _pynewton.Vehicle_Reset(*args)

    def AddTire(*args):
        """
        AddTire(self, dFloat matrix, dFloat pin, dFloat mass, dFloat width, 
            dFloat radius, dFloat suspensionShock, dFloat suspensionSpring, 
            dFloat suspensionLength, 
            PyObject userData, int collisionID) -> Tire

        1
        """
        return _pynewton.Vehicle_AddTire(*args)

    def RemoveTire(*args):
        """
        RemoveTire(self, Tire tire)

        1
        """
        return _pynewton.Vehicle_RemoveTire(*args)

    def OnCallback(*args):
        """
        OnCallback(self)

        1
        """
        return _pynewton.Vehicle_OnCallback(*args)

    def GetFirstTire(*args):
        """
        GetFirstTire(self) -> Tire

        1
        """
        return _pynewton.Vehicle_GetFirstTire(*args)

    def GetNextTireID(*args):
        """
        GetNextTireID(self, Tire tireID) -> Tire

        1
        """
        return _pynewton.Vehicle_GetNextTireID(*args)

    def __disown__(self):
        self.thisown = 0
        _pynewton.disown_Vehicle(self)
        return weakref_proxy(self)

class VehiclePtr(Vehicle):
    def __init__(self, this):
        _swig_setattr(self, Vehicle, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Vehicle, 'thisown', 0)
        self.__class__ = Vehicle
_pynewton.Vehicle_swigregister(VehiclePtr)


def GetEulerAngle(*args):
    """
    GetEulerAngle(dFloat matrix) -> PyObject

    1
    """
    return _pynewton.GetEulerAngle(*args)

def SetEulerAngle(*args):
    """
    SetEulerAngle(dFloat angle) -> PyObject

    1
    """
    return _pynewton.SetEulerAngle(*args)
class HeightField(_object):
    """1"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HeightField, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HeightField, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ HeightField instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, World world, int size, dFloat cellSize, PyObject height_values) -> HeightField

        1
        """
        _swig_setattr(self, HeightField, 'this', _pynewton.new_HeightField(*args))
        _swig_setattr(self, HeightField, 'thisown', 1)
    def __del__(self, destroy=_pynewton.delete_HeightField):
        """
        __del__(self)

        1
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetMatrix(*args):
        """
        SetMatrix(self, dFloat matrix)

        1
        """
        return _pynewton.HeightField_SetMatrix(*args)

    def GetMatrix(*args):
        """
        GetMatrix(self) -> PyObject

        1
        """
        return _pynewton.HeightField_GetMatrix(*args)

    def CellSize(*args):
        """
        CellSize(self) -> dFloat

        1
        """
        return _pynewton.HeightField_CellSize(*args)

    def Size(*args):
        """
        Size(self) -> int

        1
        """
        return _pynewton.HeightField_Size(*args)


class HeightFieldPtr(HeightField):
    def __init__(self, this):
        _swig_setattr(self, HeightField, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, HeightField, 'thisown', 0)
        self.__class__ = HeightField
_pynewton.HeightField_swigregister(HeightFieldPtr)



