# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _terrain
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


class TerrainMap(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainMap, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainMap, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _terrain.new_TerrainMap(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainMap
    __del__ = lambda self : None;
    def UpdateLOD(*args): return _terrain.TerrainMap_UpdateLOD(*args)
    def ArrangePatches(*args): return _terrain.TerrainMap_ArrangePatches(*args)
    def PatchesAcross(*args): return _terrain.TerrainMap_PatchesAcross(*args)
    def GetWestNeighborOf(*args): return _terrain.TerrainMap_GetWestNeighborOf(*args)
    def GetEastNeighborOf(*args): return _terrain.TerrainMap_GetEastNeighborOf(*args)
    def GetNorthNeighborOf(*args): return _terrain.TerrainMap_GetNorthNeighborOf(*args)
    def GetSouthNeighborOf(*args): return _terrain.TerrainMap_GetSouthNeighborOf(*args)
    def ClearFlags(*args): return _terrain.TerrainMap_ClearFlags(*args)
    def Update(*args): return _terrain.TerrainMap_Update(*args)
    def ResetLOD(*args): return _terrain.TerrainMap_ResetLOD(*args)
    def GetPatchAt(*args): return _terrain.TerrainMap_GetPatchAt(*args)
    def SetLODOnPatchAt(*args): return _terrain.TerrainMap_SetLODOnPatchAt(*args)
    def Spacing(*args): return _terrain.TerrainMap_Spacing(*args)
    def Reindex(*args): return _terrain.TerrainMap_Reindex(*args)
    def DrawAll(*args): return _terrain.TerrainMap_DrawAll(*args)
TerrainMap_swigregister = _terrain.TerrainMap_swigregister
TerrainMap_swigregister(TerrainMap)

PI = _terrain.PI
class Vector2D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vector2D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Vector2D, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _terrain.Vector2D_x_set
    __swig_getmethods__["x"] = _terrain.Vector2D_x_get
    if _newclass:x = property(_terrain.Vector2D_x_get, _terrain.Vector2D_x_set)
    __swig_setmethods__["y"] = _terrain.Vector2D_y_set
    __swig_getmethods__["y"] = _terrain.Vector2D_y_get
    if _newclass:y = property(_terrain.Vector2D_y_get, _terrain.Vector2D_y_set)
    def __init__(self, *args): 
        this = _terrain.new_Vector2D(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_Vector2D
    __del__ = lambda self : None;
Vector2D_swigregister = _terrain.Vector2D_swigregister
Vector2D_swigregister(Vector2D)

class Vector3D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vector3D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Vector3D, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _terrain.Vector3D_x_set
    __swig_getmethods__["x"] = _terrain.Vector3D_x_get
    if _newclass:x = property(_terrain.Vector3D_x_get, _terrain.Vector3D_x_set)
    __swig_setmethods__["y"] = _terrain.Vector3D_y_set
    __swig_getmethods__["y"] = _terrain.Vector3D_y_get
    if _newclass:y = property(_terrain.Vector3D_y_get, _terrain.Vector3D_y_set)
    __swig_setmethods__["z"] = _terrain.Vector3D_z_set
    __swig_getmethods__["z"] = _terrain.Vector3D_z_get
    if _newclass:z = property(_terrain.Vector3D_z_get, _terrain.Vector3D_z_set)
    def __init__(self, *args): 
        this = _terrain.new_Vector3D(*args)
        try: self.this.append(this)
        except: self.this = this
    def Dot(*args): return _terrain.Vector3D_Dot(*args)
    def Cross(*args): return _terrain.Vector3D_Cross(*args)
    def Mag(*args): return _terrain.Vector3D_Mag(*args)
    def MagSq(*args): return _terrain.Vector3D_MagSq(*args)
    def Length(*args): return _terrain.Vector3D_Length(*args)
    def LengthSq(*args): return _terrain.Vector3D_LengthSq(*args)
    def Normalized(*args): return _terrain.Vector3D_Normalized(*args)
    def __sub__(*args): return _terrain.Vector3D___sub__(*args)
    def __add__(*args): return _terrain.Vector3D___add__(*args)
    def __mul__(*args): return _terrain.Vector3D___mul__(*args)
    def __div__(*args): return _terrain.Vector3D___div__(*args)
    def __imul__(*args): return _terrain.Vector3D___imul__(*args)
    def __idiv__(*args): return _terrain.Vector3D___idiv__(*args)
    def __iadd__(*args): return _terrain.Vector3D___iadd__(*args)
    def __isub__(*args): return _terrain.Vector3D___isub__(*args)
    __swig_destroy__ = _terrain.delete_Vector3D
    __del__ = lambda self : None;
Vector3D_swigregister = _terrain.Vector3D_swigregister
Vector3D_swigregister(Vector3D)

class Plane(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Plane, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Plane, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _terrain.Plane_a_set
    __swig_getmethods__["a"] = _terrain.Plane_a_get
    if _newclass:a = property(_terrain.Plane_a_get, _terrain.Plane_a_set)
    __swig_setmethods__["b"] = _terrain.Plane_b_set
    __swig_getmethods__["b"] = _terrain.Plane_b_get
    if _newclass:b = property(_terrain.Plane_b_get, _terrain.Plane_b_set)
    __swig_setmethods__["c"] = _terrain.Plane_c_set
    __swig_getmethods__["c"] = _terrain.Plane_c_get
    if _newclass:c = property(_terrain.Plane_c_get, _terrain.Plane_c_set)
    __swig_setmethods__["d"] = _terrain.Plane_d_set
    __swig_getmethods__["d"] = _terrain.Plane_d_get
    if _newclass:d = property(_terrain.Plane_d_get, _terrain.Plane_d_set)
    def __init__(self, *args): 
        this = _terrain.new_Plane(*args)
        try: self.this.append(this)
        except: self.this = this
    def DistanceToPoint(*args): return _terrain.Plane_DistanceToPoint(*args)
    def IntersectLine(*args): return _terrain.Plane_IntersectLine(*args)
    __swig_getmethods__["TriangleNormal"] = lambda x: _terrain.Plane_TriangleNormal
    if _newclass:TriangleNormal = staticmethod(_terrain.Plane_TriangleNormal)
    __swig_getmethods__["IntersectLineWithTriangle"] = lambda x: _terrain.Plane_IntersectLineWithTriangle
    if _newclass:IntersectLineWithTriangle = staticmethod(_terrain.Plane_IntersectLineWithTriangle)
    __swig_destroy__ = _terrain.delete_Plane
    __del__ = lambda self : None;
Plane_swigregister = _terrain.Plane_swigregister
Plane_swigregister(Plane)
Plane_TriangleNormal = _terrain.Plane_TriangleNormal
Plane_IntersectLineWithTriangle = _terrain.Plane_IntersectLineWithTriangle

class BoundingVolume(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundingVolume, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BoundingVolume, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _terrain.new_BoundingVolume(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_BoundingVolume
    __del__ = lambda self : None;
BoundingVolume_swigregister = _terrain.BoundingVolume_swigregister
BoundingVolume_swigregister(BoundingVolume)

class BoundingBox(BoundingVolume):
    __swig_setmethods__ = {}
    for _s in [BoundingVolume]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundingBox, name, value)
    __swig_getmethods__ = {}
    for _s in [BoundingVolume]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BoundingBox, name)
    __repr__ = _swig_repr
    __swig_setmethods__["BoxCoords"] = _terrain.BoundingBox_BoxCoords_set
    __swig_getmethods__["BoxCoords"] = _terrain.BoundingBox_BoxCoords_get
    if _newclass:BoxCoords = property(_terrain.BoundingBox_BoxCoords_get, _terrain.BoundingBox_BoxCoords_set)
    def __init__(self, *args): 
        this = _terrain.new_BoundingBox(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_BoundingBox
    __del__ = lambda self : None;
BoundingBox_swigregister = _terrain.BoundingBox_swigregister
BoundingBox_swigregister(BoundingBox)

class BoundingSphere(BoundingVolume):
    __swig_setmethods__ = {}
    for _s in [BoundingVolume]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundingSphere, name, value)
    __swig_getmethods__ = {}
    for _s in [BoundingVolume]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BoundingSphere, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Radius"] = _terrain.BoundingSphere_Radius_set
    __swig_getmethods__["Radius"] = _terrain.BoundingSphere_Radius_get
    if _newclass:Radius = property(_terrain.BoundingSphere_Radius_get, _terrain.BoundingSphere_Radius_set)
    __swig_setmethods__["location"] = _terrain.BoundingSphere_location_set
    __swig_getmethods__["location"] = _terrain.BoundingSphere_location_get
    if _newclass:location = property(_terrain.BoundingSphere_location_get, _terrain.BoundingSphere_location_set)
    def x(*args): return _terrain.BoundingSphere_x(*args)
    def y(*args): return _terrain.BoundingSphere_y(*args)
    def z(*args): return _terrain.BoundingSphere_z(*args)
    def __init__(self, *args): 
        this = _terrain.new_BoundingSphere(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_BoundingSphere
    __del__ = lambda self : None;
BoundingSphere_swigregister = _terrain.BoundingSphere_swigregister
BoundingSphere_swigregister(BoundingSphere)

class BoundingCube(BoundingVolume):
    __swig_setmethods__ = {}
    for _s in [BoundingVolume]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundingCube, name, value)
    __swig_getmethods__ = {}
    for _s in [BoundingVolume]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BoundingCube, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _terrain.BoundingCube_x_set
    __swig_getmethods__["x"] = _terrain.BoundingCube_x_get
    if _newclass:x = property(_terrain.BoundingCube_x_get, _terrain.BoundingCube_x_set)
    __swig_setmethods__["y"] = _terrain.BoundingCube_y_set
    __swig_getmethods__["y"] = _terrain.BoundingCube_y_get
    if _newclass:y = property(_terrain.BoundingCube_y_get, _terrain.BoundingCube_y_set)
    __swig_setmethods__["z"] = _terrain.BoundingCube_z_set
    __swig_getmethods__["z"] = _terrain.BoundingCube_z_get
    if _newclass:z = property(_terrain.BoundingCube_z_get, _terrain.BoundingCube_z_set)
    __swig_setmethods__["Size"] = _terrain.BoundingCube_Size_set
    __swig_getmethods__["Size"] = _terrain.BoundingCube_Size_get
    if _newclass:Size = property(_terrain.BoundingCube_Size_get, _terrain.BoundingCube_Size_set)
    def __init__(self, *args): 
        this = _terrain.new_BoundingCube(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_BoundingCube
    __del__ = lambda self : None;
BoundingCube_swigregister = _terrain.BoundingCube_swigregister
BoundingCube_swigregister(BoundingCube)

class AABB(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AABB, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AABB, name)
    __repr__ = _swig_repr
    __swig_setmethods__["min"] = _terrain.AABB_min_set
    __swig_getmethods__["min"] = _terrain.AABB_min_get
    if _newclass:min = property(_terrain.AABB_min_get, _terrain.AABB_min_set)
    __swig_setmethods__["max"] = _terrain.AABB_max_set
    __swig_getmethods__["max"] = _terrain.AABB_max_get
    if _newclass:max = property(_terrain.AABB_max_get, _terrain.AABB_max_set)
    def __init__(self, *args): 
        this = _terrain.new_AABB(*args)
        try: self.this.append(this)
        except: self.this = this
    def InterpolateBox(*args): return _terrain.AABB_InterpolateBox(*args)
    __swig_destroy__ = _terrain.delete_AABB
    __del__ = lambda self : None;
AABB_swigregister = _terrain.AABB_swigregister
AABB_swigregister(AABB)

class TerrainTriangle(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainTriangle, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainTriangle, name)
    __repr__ = _swig_repr
    __swig_setmethods__["v1"] = _terrain.TerrainTriangle_v1_set
    __swig_getmethods__["v1"] = _terrain.TerrainTriangle_v1_get
    if _newclass:v1 = property(_terrain.TerrainTriangle_v1_get, _terrain.TerrainTriangle_v1_set)
    __swig_setmethods__["v2"] = _terrain.TerrainTriangle_v2_set
    __swig_getmethods__["v2"] = _terrain.TerrainTriangle_v2_get
    if _newclass:v2 = property(_terrain.TerrainTriangle_v2_get, _terrain.TerrainTriangle_v2_set)
    __swig_setmethods__["v3"] = _terrain.TerrainTriangle_v3_set
    __swig_getmethods__["v3"] = _terrain.TerrainTriangle_v3_get
    if _newclass:v3 = property(_terrain.TerrainTriangle_v3_get, _terrain.TerrainTriangle_v3_set)
    def GetPlane(*args): return _terrain.TerrainTriangle_GetPlane(*args)
    def __init__(self, *args): 
        this = _terrain.new_TerrainTriangle(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainTriangle
    __del__ = lambda self : None;
TerrainTriangle_swigregister = _terrain.TerrainTriangle_swigregister
TerrainTriangle_swigregister(TerrainTriangle)

class TrianglePair(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TrianglePair, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TrianglePair, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _terrain.TrianglePair_a_set
    __swig_getmethods__["a"] = _terrain.TrianglePair_a_get
    if _newclass:a = property(_terrain.TrianglePair_a_get, _terrain.TrianglePair_a_set)
    __swig_setmethods__["b"] = _terrain.TrianglePair_b_set
    __swig_getmethods__["b"] = _terrain.TrianglePair_b_get
    if _newclass:b = property(_terrain.TrianglePair_b_get, _terrain.TrianglePair_b_set)
    def A(*args): return _terrain.TrianglePair_A(*args)
    def B(*args): return _terrain.TrianglePair_B(*args)
    def __init__(self, *args): 
        this = _terrain.new_TrianglePair(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TrianglePair
    __del__ = lambda self : None;
TrianglePair_swigregister = _terrain.TrianglePair_swigregister
TrianglePair_swigregister(TrianglePair)

class HeightGrid(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HeightGrid, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HeightGrid, name)
    __repr__ = _swig_repr
    def Size(*args): return _terrain.HeightGrid_Size(*args)
    def SquaresAcross(*args): return _terrain.HeightGrid_SquaresAcross(*args)
    def HalfSize(*args): return _terrain.HeightGrid_HalfSize(*args)
    def HighestHeight(*args): return _terrain.HeightGrid_HighestHeight(*args)
    def LowestHeight(*args): return _terrain.HeightGrid_LowestHeight(*args)
    def HeightData(*args): return _terrain.HeightGrid_HeightData(*args)
    def __init__(self, *args): 
        this = _terrain.new_HeightGrid(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_HeightGrid
    __del__ = lambda self : None;
    def Init(*args): return _terrain.HeightGrid_Init(*args)
    def SetHeight(*args): return _terrain.HeightGrid_SetHeight(*args)
    def GetHeight(*args): return _terrain.HeightGrid_GetHeight(*args)
    def CalculateHeightExtremities(*args): return _terrain.HeightGrid_CalculateHeightExtremities(*args)
    def GetTriangleAtPosition(*args): return _terrain.HeightGrid_GetTriangleAtPosition(*args)
    def GetNormalAtPosition(*args): return _terrain.HeightGrid_GetNormalAtPosition(*args)
    def GetInterpolatedHeight(*args): return _terrain.HeightGrid_GetInterpolatedHeight(*args)
    def GetTrianglesAtGridLocation(*args): return _terrain.HeightGrid_GetTrianglesAtGridLocation(*args)
    def GetAllTriangles(*args): return _terrain.HeightGrid_GetAllTriangles(*args)
    def HalfLOD(*args): return _terrain.HeightGrid_HalfLOD(*args)
    def DoubleLOD(*args): return _terrain.HeightGrid_DoubleLOD(*args)
    def Duplicate(*args): return _terrain.HeightGrid_Duplicate(*args)
    def MakeSimplificationTarget(*args): return _terrain.HeightGrid_MakeSimplificationTarget(*args)
    def MakeHeightGridFromSubset(*args): return _terrain.HeightGrid_MakeHeightGridFromSubset(*args)
    def SubdivideIntoQuads(*args): return _terrain.HeightGrid_SubdivideIntoQuads(*args)
    def Fracture(*args): return _terrain.HeightGrid_Fracture(*args)
HeightGrid_swigregister = _terrain.HeightGrid_swigregister
HeightGrid_swigregister(HeightGrid)

class LODDescription(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, LODDescription, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, LODDescription, name)
    __repr__ = _swig_repr
    __swig_setmethods__["westLOD"] = _terrain.LODDescription_westLOD_set
    __swig_getmethods__["westLOD"] = _terrain.LODDescription_westLOD_get
    if _newclass:westLOD = property(_terrain.LODDescription_westLOD_get, _terrain.LODDescription_westLOD_set)
    __swig_setmethods__["eastLOD"] = _terrain.LODDescription_eastLOD_set
    __swig_getmethods__["eastLOD"] = _terrain.LODDescription_eastLOD_get
    if _newclass:eastLOD = property(_terrain.LODDescription_eastLOD_get, _terrain.LODDescription_eastLOD_set)
    __swig_setmethods__["southLOD"] = _terrain.LODDescription_southLOD_set
    __swig_getmethods__["southLOD"] = _terrain.LODDescription_southLOD_get
    if _newclass:southLOD = property(_terrain.LODDescription_southLOD_get, _terrain.LODDescription_southLOD_set)
    __swig_setmethods__["northLOD"] = _terrain.LODDescription_northLOD_set
    __swig_getmethods__["northLOD"] = _terrain.LODDescription_northLOD_get
    if _newclass:northLOD = property(_terrain.LODDescription_northLOD_get, _terrain.LODDescription_northLOD_set)
    __swig_setmethods__["LOD"] = _terrain.LODDescription_LOD_set
    __swig_getmethods__["LOD"] = _terrain.LODDescription_LOD_get
    if _newclass:LOD = property(_terrain.LODDescription_LOD_get, _terrain.LODDescription_LOD_set)
    def SetDefaults(*args): return _terrain.LODDescription_SetDefaults(*args)
    def __init__(self, *args): 
        this = _terrain.new_LODDescription(*args)
        try: self.this.append(this)
        except: self.this = this
    def LODOfPatch(*args): return _terrain.LODDescription_LODOfPatch(*args)
    def LoadValuesFromPatch(*args): return _terrain.LODDescription_LoadValuesFromPatch(*args)
    def IsConsistentWithPatch(*args): return _terrain.LODDescription_IsConsistentWithPatch(*args)
    def Equals(*args): return _terrain.LODDescription_Equals(*args)
    __swig_destroy__ = _terrain.delete_LODDescription
    __del__ = lambda self : None;
LODDescription_swigregister = _terrain.LODDescription_swigregister
LODDescription_swigregister(LODDescription)

class TerrainIndexBlock(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainIndexBlock, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainIndexBlock, name)
    __repr__ = _swig_repr
    __swig_setmethods__["m_westSimplificationLevel"] = _terrain.TerrainIndexBlock_m_westSimplificationLevel_set
    __swig_getmethods__["m_westSimplificationLevel"] = _terrain.TerrainIndexBlock_m_westSimplificationLevel_get
    if _newclass:m_westSimplificationLevel = property(_terrain.TerrainIndexBlock_m_westSimplificationLevel_get, _terrain.TerrainIndexBlock_m_westSimplificationLevel_set)
    __swig_setmethods__["m_eastSimplificationLevel"] = _terrain.TerrainIndexBlock_m_eastSimplificationLevel_set
    __swig_getmethods__["m_eastSimplificationLevel"] = _terrain.TerrainIndexBlock_m_eastSimplificationLevel_get
    if _newclass:m_eastSimplificationLevel = property(_terrain.TerrainIndexBlock_m_eastSimplificationLevel_get, _terrain.TerrainIndexBlock_m_eastSimplificationLevel_set)
    __swig_setmethods__["m_northSimplificationLevel"] = _terrain.TerrainIndexBlock_m_northSimplificationLevel_set
    __swig_getmethods__["m_northSimplificationLevel"] = _terrain.TerrainIndexBlock_m_northSimplificationLevel_get
    if _newclass:m_northSimplificationLevel = property(_terrain.TerrainIndexBlock_m_northSimplificationLevel_get, _terrain.TerrainIndexBlock_m_northSimplificationLevel_set)
    __swig_setmethods__["m_southSimplificationLevel"] = _terrain.TerrainIndexBlock_m_southSimplificationLevel_set
    __swig_getmethods__["m_southSimplificationLevel"] = _terrain.TerrainIndexBlock_m_southSimplificationLevel_get
    if _newclass:m_southSimplificationLevel = property(_terrain.TerrainIndexBlock_m_southSimplificationLevel_get, _terrain.TerrainIndexBlock_m_southSimplificationLevel_set)
    def __init__(self, *args): 
        this = _terrain.new_TerrainIndexBlock(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainIndexBlock
    __del__ = lambda self : None;
    def IndexCount(*args): return _terrain.TerrainIndexBlock_IndexCount(*args)
    def TriangleCount(*args): return _terrain.TerrainIndexBlock_TriangleCount(*args)
    def Indices(*args): return _terrain.TerrainIndexBlock_Indices(*args)
    def GetIndexForCoordinate(*args): return _terrain.TerrainIndexBlock_GetIndexForCoordinate(*args)
    def MakeBasicIndices(*args): return _terrain.TerrainIndexBlock_MakeBasicIndices(*args)
    def UpdateIndices(*args): return _terrain.TerrainIndexBlock_UpdateIndices(*args)
TerrainIndexBlock_swigregister = _terrain.TerrainIndexBlock_swigregister
TerrainIndexBlock_swigregister(TerrainIndexBlock)

class TerrainPatch(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainPatch, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainPatch, name)
    __repr__ = _swig_repr
    def ClearFlags(*args): return _terrain.TerrainPatch_ClearFlags(*args)
    def Center(*args): return _terrain.TerrainPatch_Center(*args)
    def ParentTerrain(*args): return _terrain.TerrainPatch_ParentTerrain(*args)
    def Row(*args): return _terrain.TerrainPatch_Row(*args)
    def Column(*args): return _terrain.TerrainPatch_Column(*args)
    def LOD(*args): return _terrain.TerrainPatch_LOD(*args)
    def SpacingBetweenVertices(*args): return _terrain.TerrainPatch_SpacingBetweenVertices(*args)
    def Offset(*args): return _terrain.TerrainPatch_Offset(*args)
    def WorldLength(*args): return _terrain.TerrainPatch_WorldLength(*args)
    def Rescale(*args): return _terrain.TerrainPatch_Rescale(*args)
    def __init__(self, *args): 
        this = _terrain.new_TerrainPatch(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainPatch
    __del__ = lambda self : None;
    def Init(*args): return _terrain.TerrainPatch_Init(*args)
    def Cleanup(*args): return _terrain.TerrainPatch_Cleanup(*args)
    def SetIndexSimplification(*args): return _terrain.TerrainPatch_SetIndexSimplification(*args)
    def SynchronizeDescWithState(*args): return _terrain.TerrainPatch_SynchronizeDescWithState(*args)
    def CheckForLODConsistency(*args): return _terrain.TerrainPatch_CheckForLODConsistency(*args)
    def GetHeightWithPercentage(*args): return _terrain.TerrainPatch_GetHeightWithPercentage(*args)
    def GetTargetHeightWithPercentage(*args): return _terrain.TerrainPatch_GetTargetHeightWithPercentage(*args)
    def RecalcIndexBuffer(*args): return _terrain.TerrainPatch_RecalcIndexBuffer(*args)
    def Refresh(*args): return _terrain.TerrainPatch_Refresh(*args)
    def Draw(*args): return _terrain.TerrainPatch_Draw(*args)
    def DrawLevel(*args): return _terrain.TerrainPatch_DrawLevel(*args)
TerrainPatch_swigregister = _terrain.TerrainPatch_swigregister
TerrainPatch_swigregister(TerrainPatch)

class TerrainMipMapLevel(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainMipMapLevel, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainMipMapLevel, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _terrain.new_TerrainMipMapLevel(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainMipMapLevel
    __del__ = lambda self : None;
    def Init(*args): return _terrain.TerrainMipMapLevel_Init(*args)
    def Cleanup(*args): return _terrain.TerrainMipMapLevel_Cleanup(*args)
    def Length(*args): return _terrain.TerrainMipMapLevel_Length(*args)
    def WorldLength(*args): return _terrain.TerrainMipMapLevel_WorldLength(*args)
    def Spacing(*args): return _terrain.TerrainMipMapLevel_Spacing(*args)
    def Interpolation(*args): return _terrain.TerrainMipMapLevel_Interpolation(*args)
    def GetCurrentHeight(*args): return _terrain.TerrainMipMapLevel_GetCurrentHeight(*args)
    def GetTargetHeight(*args): return _terrain.TerrainMipMapLevel_GetTargetHeight(*args)
    def GetCurrentInterpolatedHeight(*args): return _terrain.TerrainMipMapLevel_GetCurrentInterpolatedHeight(*args)
    def MakeVertices(*args): return _terrain.TerrainMipMapLevel_MakeVertices(*args)
    def InitVertexHeights(*args): return _terrain.TerrainMipMapLevel_InitVertexHeights(*args)
    def SetIndexBlockSimplification(*args): return _terrain.TerrainMipMapLevel_SetIndexBlockSimplification(*args)
    def UpdateIndices(*args): return _terrain.TerrainMipMapLevel_UpdateIndices(*args)
    def UpdateVertexHeights(*args): return _terrain.TerrainMipMapLevel_UpdateVertexHeights(*args)
    def Draw(*args): return _terrain.TerrainMipMapLevel_Draw(*args)
TerrainMipMapLevel_swigregister = _terrain.TerrainMipMapLevel_swigregister
TerrainMipMapLevel_swigregister(TerrainMipMapLevel)

class TerrainLODVertex(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TerrainLODVertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TerrainLODVertex, name)
    __repr__ = _swig_repr
    __swig_setmethods__["v"] = _terrain.TerrainLODVertex_v_set
    __swig_getmethods__["v"] = _terrain.TerrainLODVertex_v_get
    if _newclass:v = property(_terrain.TerrainLODVertex_v_get, _terrain.TerrainLODVertex_v_set)
    __swig_setmethods__["tex0"] = _terrain.TerrainLODVertex_tex0_set
    __swig_getmethods__["tex0"] = _terrain.TerrainLODVertex_tex0_get
    if _newclass:tex0 = property(_terrain.TerrainLODVertex_tex0_get, _terrain.TerrainLODVertex_tex0_set)
    __swig_setmethods__["tex1"] = _terrain.TerrainLODVertex_tex1_set
    __swig_getmethods__["tex1"] = _terrain.TerrainLODVertex_tex1_get
    if _newclass:tex1 = property(_terrain.TerrainLODVertex_tex1_get, _terrain.TerrainLODVertex_tex1_set)
    __swig_setmethods__["tex2"] = _terrain.TerrainLODVertex_tex2_set
    __swig_getmethods__["tex2"] = _terrain.TerrainLODVertex_tex2_get
    if _newclass:tex2 = property(_terrain.TerrainLODVertex_tex2_get, _terrain.TerrainLODVertex_tex2_set)
    def __init__(self, *args): 
        this = _terrain.new_TerrainLODVertex(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_TerrainLODVertex
    __del__ = lambda self : None;
TerrainLODVertex_swigregister = _terrain.TerrainLODVertex_swigregister
TerrainLODVertex_swigregister(TerrainLODVertex)

class VertexBuffer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VertexBuffer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VertexBuffer, name)
    __repr__ = _swig_repr
    __swig_setmethods__["vertexArray"] = _terrain.VertexBuffer_vertexArray_set
    __swig_getmethods__["vertexArray"] = _terrain.VertexBuffer_vertexArray_get
    if _newclass:vertexArray = property(_terrain.VertexBuffer_vertexArray_get, _terrain.VertexBuffer_vertexArray_set)
    __swig_setmethods__["nVertices"] = _terrain.VertexBuffer_nVertices_set
    __swig_getmethods__["nVertices"] = _terrain.VertexBuffer_nVertices_get
    if _newclass:nVertices = property(_terrain.VertexBuffer_nVertices_get, _terrain.VertexBuffer_nVertices_set)
    def __init__(self, *args): 
        this = _terrain.new_VertexBuffer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _terrain.delete_VertexBuffer
    __del__ = lambda self : None;
    def Init(*args): return _terrain.VertexBuffer_Init(*args)
VertexBuffer_swigregister = _terrain.VertexBuffer_swigregister
VertexBuffer_swigregister(VertexBuffer)



