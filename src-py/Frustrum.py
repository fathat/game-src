from cgkit.cgtypes import *
from Graphics import GraphicsCard
import ListFunctions
import mathhat

class Plane(object):
    def __init__(self, a, b, c, d ):
        self.a = a
        self.b = b
        self.c = c
        self.d = d

    def __repr__(self):
        return "<Plane: %s %s %s %s>" % ( str( self.a), str(self.b), str(self.c), str(self.d) )

    def normal(self):
        return vec3(self.a, self.b, self.c)

    def normalize(self):
        length = self.normal().length()
        self.a /= length
        self.b /= length
        self.c /= length
        self.d /= length

    def distanceToPoint( self, point ):
        return (self.normal() * point) - self.d


class Frustrum(object):
    """A frustrum"""

    def __init__(self, camera):
        """Initialize the frustrum on a camera"""
        self.planes = []
        self.camera = camera

    def _constructClippingPlane( self, viewProj, positive, axis):
        """Construct a clipping plane. This function doesn't make much sense
        to use on its own -- its used by update instead."""
        if positive: scale = 1
        else: scale = -1

        return Plane(viewProj[0,3] + scale*viewProj[0, axis],
                 viewProj[1,3] + scale*viewProj[1, axis],
                 viewProj[2,3] + scale*viewProj[2, axis],
                 viewProj[3,3] + scale*viewProj[3, axis] )

    def update(self):
        proj_m =GraphicsCard.getProjectionMatrix()
        #mat4(ListFunctions.flattenListOfLists( glGetDoublev( GL_PROJECTION_MATRIX ) ) )
        modl_m = self.camera.getViewMatrix()

        #proj = proj_m.transpose().toList()
        #modl = modl_m.toList()
        m = (proj_m.transpose() * modl_m).toList()

        """m = range(16)
        m[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
        m[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
        m[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
        m[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

        m[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
        m[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
        m[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
        m[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

        m[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
        m[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
        m[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
        m[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

        m[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
        m[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
        m[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
        m[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];"""

        # Create clipping planes
        self.planes = []

        #right
        p = Plane(0,0,0,0)
        p.a = m[3]-m[0]
        p.b = m[7]-m[4]
        p.c = m[11]-m[8]
        p.d = m[15]-m[12]
        self.planes.append( p )

        #left
        p = Plane(0,0,0,0)
        p.a = m[3]+m[0]
        p.b = m[7]+m[4]
        p.c = m[11]+m[8]
        p.d = m[15]+m[12]
        self.planes.append( p )

        #bottom
        p = Plane(0,0,0,0)
        p.a = m[3]+m[1]
        p.b = m[7]+m[5]
        p.c = m[11]+m[9]
        p.d = m[15]+m[13]
        self.planes.append( p )

        #top
        p = Plane(0,0,0,0)
        p.a = m[3]-m[1]
        p.b = m[7]-m[5]
        p.c = m[11]-m[9]
        p.d = m[15]-m[13]
        self.planes.append( p )

        #near
        p = Plane(0,0,0,0)
        p.a = m[3]+m[2]
        p.b = m[7]+m[6]
        p.c = m[11]+m[10]
        p.d = m[15]+m[14]
        #self.planes.append( p )

        #far
        p = Plane(0,0,0,0)
        p.a = m[3]-m[2]
        p.b = m[7]-m[6]
        p.c = m[11]-m[10]
        p.d = m[15]-m[14]
        #self.planes.append( p )

        #Normalize the planes
        #print "-----"
        for p in self.planes:
            p.normalize()

    def boxInFrustrum( self, bbmin, bbmax) :
        myMin = vec3()
        myMax = vec3()
        intersect = False

        for p in self.planes:
            if p.a < 0:
                myMin.x, myMax.x = bbMin.x, bbMax.x
            else:
                myMin.x, myMax.x = bbMax.x, bbMin.x #Note swapped min and max

            if p.b < 0:
                myMin.y, myMax.y = bbMin.y, bbMax.y
            else:
                myMin.y, myMax.y = bbMax.y, bbMin.y #Note swapped min and max

            if p.c < 0:
                myMin.z, myMax.z = bbMin.z, bbMax.z
            else:
                myMin.z, myMax.z = bbMax.z, bbMin.z #Note swapped min and max

            if p.distanceToPoint( myMin ) < 0.0:
                return 0
            if p.distanceToPoint( myMax ) <= 0.0:
                intersect = True
        if intersect:
            return 1
        return 2

    def pointInFrustrum( self, point ):
        for p in self.planes:
            if p.a * point.x + p.b*point.y + p.c*point.y + p.d <= 0:
                return False
        return True

    def sphereInFrustrum( self, center, radius ):
        for p in self.planes:
            if  p.a * center.x + p.b * center.y + p.c * center.z + p.d <= -radius:
                return False
        return True

