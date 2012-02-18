import math
import DebugDisplay
from cgkit.cgtypes import vec3, vec4, quat, mat4


class Angle(object):
    def __init__(self, degree):
        self.num = degree


def addUpTo( current, target, max_amount ):
    if target < current:
        return max( target, current-max_amount)
    else:
        return min( target, current+max_amount)

def degreeToRadian( angle ):
    return float(angle) * (math.pi/180.0)

def radianToDegree( angle ):
    return float(angle) * (180.0/math.pi)

def interpolateVectors( vecA, vecB, t ):
    r = vec3()
    diff = vecB - vecA
    r.x = vecA.x + diff.x*t
    r.y = vecA.y + diff.y*t
    r.z = vecA.z + diff.z*t
    return r

def yawAngle( vecA ):
    return math.atan2( vecA.x, vecA.z )

def neededForceForVelocity( old_velocity, desired_velocity, mass, timestep):
    diff = desired_velocity - old_velocity
    return mass*diff/timestep

def rotateVectorAroundAxis( aVector, angle, axis, invert_quaternion=False ):
    q = quaternionForAxisAngle( angle, axis)
    if invert_quaternion: q = q.inverse()
    return q.toMat3()*aVector

def quaternionForAxisAngle( angle, axis ):
    return quat().fromAngleAxis( angle, axis )

def playerMoveForce( old_velocity, yaw_in_radians, forward_vel, strafe_vel, mass, timestep, max_force ):
    q = quaternionForAxisAngle( yaw_in_radians, vec3(0,1,0) )
    relative_velocity = q.inverse().toMat3() * old_velocity
    cur_fwd    = relative_velocity.z
    cur_strafe = relative_velocity.x
    fwd_force = neededForceForVelocity( cur_fwd, forward_vel, mass, timestep)
    stf_force = neededForceForVelocity( cur_strafe, strafe_vel, mass, timestep)
    force_mag = math.sqrt( fwd_force*fwd_force + stf_force*stf_force )
    if force_mag <= 0.0001:
        return vec3(0,0,0)
    #don't let force exceed maximum
    if force_mag > max_force:
        fwd_force *= float(max_force)/float(force_mag)
        stf_force *= float(max_force)/float(force_mag)
    return q.toMat3() * vec3(stf_force, 0, fwd_force)


def transformPoint( point, matrix ):
    v = vec4(point.x, point.y, point.z, 1 )
    r = matrix*v
    return vec3( r.x, r.y, r.z )

def getTriangleNormal( v1, v2, v3 ):
    #find the normal
    a = v2 - v1
    b = v3 - v1
    return (b.cross(a)).normalize()

def yawOfVec3(v):
    return math.atan2(v.z, v.x) % (math.pi*2)

def rollOfVec3(v):
    return math.atan2(v.y, v.x) % (math.pi*2)

def pitchOfVec3(v):
    return math.atan2(v.z, v.y) % (math.pi*2)

def getMatrixYPR(m):
    v = vec3(0, 0, -1)
    tv = transformPoint(v, m)
    return yawOfVec3(tv), pitchOfVec3(tv), rollOfVec3(tv)

def getTrianglePlane( v1, v2, v3 ):
    normal = getTriangleNormal( v1, v2, v3 )
    d = -(normal.x * v1.x + normal.y * v1.y + normal.z * v1.z )
    return ( normal.x, normal.y, normal.z, d )

def buildRotationMatrix( yaw, pitch, roll ):
    m = mat4(1)
    m.rotate(yaw % (math.pi*2),   vec3( 0, 1, 0 ) )
    m.rotate(pitch % (math.pi*2), vec3( 1, 0, 0 ) )
    m.rotate(roll % (math.pi*2),  vec3( 0, 0, 1 ) )
    return m

def buildTransformMatrix( location, yaw, pitch, roll ):
    mloc   = mat4(1).translation( location )
    return mloc * buildRotationMatrix( yaw, pitch, roll )

def buildTransformMatrixQ(location, q4):
    mloc = mat4(1).translation( location )
    mrot = q4.toMat4()
    return mloc * mrot

def quaternionToEulerYawPitchRoll(q4):
    #return getMatrixYPR(q4.toMat4())
    yaw = yawOfVec3(vec3(0,0,1)*q4.toMat3())
    return yaw, 0, 0

def intersectRayWithPlane( plane, ray_origin, ray_direction ):
    plane_normal = vec3( plane[0], plane[1], plane[2] )
    d = plane[3]

    denom = (plane_normal*ray_direction )
    if denom == 0:
        return None
    num = -(plane_normal * ray_origin + d )
    t = num/denom
    return t

def unprojectRay ( x, y, invViewMatrix, projectionMatrix, screenWidth, screenHeight ):
    sx = (((2.0 * x) / screenWidth ) - 1) / projectionMatrix[ 0,0 ]
    sy = (((2.0 * y) / screenHeight) - 1) / projectionMatrix[ 1,1 ]
    sz = 1.0
    ray_direction = vec3( sx * invViewMatrix[0,0] + sy * invViewMatrix[1,0] + sz * invViewMatrix[2,0],
                          sx * invViewMatrix[0,1] + sy * invViewMatrix[1,1] + sz * invViewMatrix[2,1],
                          sx * invViewMatrix[0,2] + sy * invViewMatrix[1,2] + sz * invViewMatrix[2,2] )
    return ray_direction

if __name__ == '__main__':
    plane =  getTrianglePlane( vec3( 0, 0, 0 ), vec3( 1,0,0 ), vec3 ( 0,1,0 ) )
    print plane
    t = intersectRayWithPlane( plane, vec3( 0, 0, 3 ), vec3(0, 0, -1 ) )
    print t
