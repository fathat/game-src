import Physics
from LangExt import *

class Construction(object):

    def __init__(self, filename=None):
        self.subconstructs = []
        self.joints = []

    def _construct(self, filename):
        """make all joints and bodies and such."""

    @make_property
    def center(self):
        doc = "The center of the object"
        def fget(self): return None
        def fset(self, val): pass
        def fdel(self): pass
        return locals()


class Wheel(Construction):
    stuff = {
        'tire' : Cylinder,
        'hubcap' : Disc
    }
    connectors = {
        'wheelbase' : center,
        'hubcap' : vec3(0, 1, 0)
    }
    joints = [ Fixed( 'hubcap', center ) ]

class Car(Construction):
    stuff = {
        'front_right_wheel': Wheel,
        'front_left_wheel' : Wheel,
        'rear_left_wheel'  : Wheel,
        'rear_right_wheel' : Wheel
    }

    connectors = ConnectorsFromMesh('car.mesh')

    joints = [
        Engine('front_right_wheel', 'wheel_well_front_right'),
        Engine('front_left_wheel', 'wheel_well_front_left'),
        Engine('rear_right_wheel', 'wheel_well_rear_right'),
        Engine('rear_left_wheel', 'wheel_well_rear_left')
    ]
