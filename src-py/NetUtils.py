#NetUtils.py
from Platform import time_in_seconds

class NetMoveCommand(object):
    __slots__ = ('moveStartTime',
                 'moveEndTime',
                 'initialLocation',
                 'initialVelocity',
                 'finalLocation',
                 'finalVelocity')

    def __init__(self, ent, packet, ping):
        l = vec3(packet.x, packet.y, packet.z)
        v = vec3(packet.vx, packet.vy, packet.vz)
        self.moveStartTime = time_in_seconds()
        self.moveEndTime  = self.packetReceived + ping
        self.initialLocation = ent.location
        self.initialVelocity = ent.velocity
        self.finalLocation = l + v * (0.5*ping)
        self.finalVelocity = v

    @property
    def velocity(self):
        percent = float(time_in_seconds() - self.packetExpires) / (
        self.finalVelocity - self.initialVelocity

    @property
    def distanceError(self):
        return None
