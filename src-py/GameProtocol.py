from network.Protocol import Protocol, NetworkProtocol
import math

#!/usr/bin/env python

from network.Message import MessagePacket
from network.Fields import *

_idcount=0
def GenID():
    global _idcount
    rval = _idcount
    _idcount += 1
    return rval

class NewGameProtocol(NetworkProtocol):

    class Version(MessagePacket):
        version = ByteField(unsigned=True)

    class SetName(MessagePacket):
        username = CStringField(length=32)

    class LoadMap(MessagePacket):
        modname = CStringField(length=128)
        mapname = CStringField(length=128)

    class RconLogin(MessagePacket):
        username = CStringField(length=32)
        password = CStringField(length=32)

    class RconSend(MessagePacket):
        text = CStringField(length=256)

    class ChatMessage( MessagePacket):
        text = CStringField(length=256)

    class SpawnEnt(MessagePacket):
        typepath = CStringField(length=64)
        scriptname = CStringField(length=64)
        entID = IntField(unsigned=True)
        x = FloatField()
        y = FloatField()
        z = FloatField()
        qx = FloatField()
        qy = FloatField()
        qz = FloatField()
        qw = FloatField()

    class DeleteEnt(MessagePacket):
        entID = IntField(unsigned=True)

    class MoveEnt(MessagePacket):
        entID = IntField(unsigned=True)
        x = FloatField()
        y = FloatField()
        z = FloatField()
        qx = FloatField()
        qy = FloatField()
        qz = FloatField()
        qw = FloatField()
        vx = FloatField()
        vy = FloatField()
        vz = FloatField()

    class MoveActor(MessagePacket):
        entID = IntField(unsigned=True)
        x = FloatField()
        y = FloatField()
        z = FloatField()
        yaw = FloatField()
        pitch = FloatField()

    class SetEntLocation(MessagePacket):
        entID = IntField(unsigned=True)
        x = FloatField()
        y = FloatField()
        z = FloatField()

    class SetEntEulerRotation(MessagePacket):
        entID = IntField(unsigned=True)
        yaw   = FloatField()
        pitch = FloatField()
        roll  = FloatField()

    class AttachEnts(MessagePacket):
        firstID  = IntField(unsigned=True)
        secondID = IntField(unsigned=True)

OldGameProtocol = Protocol(
    ('Version', GenID(),
        ('version', 'B')),
    ('SetName', GenID(),
        ('username', '32s')),
    ('LoadMap', GenID(),
        ('modname', '128s'),
        ('mapname', '128s') ),
    ( 'RconLogin', GenID(),
        ('username', '32s'),
        ('password', '32s')),
    ( 'RconSend', GenID(),
        ('text', '255s')),
    ('AcceptConnect', GenID()),
    ('ConnectionDeclined', GenID(),
        ('reason', '255s')),
    ('ChatMessage', GenID(),
        ('message', '255s')),
    ('SpawnEnt', GenID(),
        ('typepath', '64s'),
        ('scriptname', '64s'),
        ('entID', 'I'),
        ('x','f'),
        ('y','f'),
        ('z','f'),
        ('qx', 'f'),
        ('qy', 'f'),
        ('qz', 'f'),
        ('qw', 'f')),
    ('DeleteEnt', GenID(),
        ('entID', 'I')),
    ('MoveEnt', GenID(),
        ('entID', 'I'),
        ('x','f'),
        ('y','f'),
        ('z','f'),
        ('qx', 'f'),
        ('qy', 'f'),
        ('qz', 'f'),
        ('qw', 'f'),
        ('vx', 'f'),
        ('vy', 'f'),
        ('vz', 'f')),
    ('AttachEnts', GenID(),
        ('firstID', 'I'),
        ('secondID', 'I'))
    )

GameProtocol = NewGameProtocol

def RadianToByte( angle ):
    return int( (angle / (math.pi*2) ) * 255) % 255

def ByteToRadian( angle ):
    return (float(angle)/255.0) * (math.pi*2)

def test():
    bytestr = str(GameProtocol.ChatMessage("hay guys"))
    print "Chat Message:"
    print GameProtocol.ChatMessage.unpack(bytestr)
    bytestr = str(GameProtocol.SpawnEnt("RobotJesus", 10, -15, 20))
    print "Spawn Message"
    print GameProtocol.read( bytestr)

if __name__=='__main__': test()
