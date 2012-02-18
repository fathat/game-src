#!/usr/bin/env python

from Protocol import Protocol
from Message import CreateMsgType

_idcount=0
def GenID():
    global _idcount
    rval = _idcount
    _idcount += 1
    return rval

GameProtocol = Protocol(
    ('Version', GenID(),
        ('version', 'B')),
    ('AcceptConnect', GenID()),
    ('ConnectionDeclined', GenID(),
        ('reason', '255s')),
    ('ChatMessage', GenID(),
        ('message', '255s')),
    ('SpawnEnt', GenID(),
        ('typename', '32s'),
        ('x','f'),
        ('y','f'),
        ('z','f')),
    ('SetName', GenID(),
        ('username', '32s')))

def test():
    bytestr = GameProtocol.ChatMessage("hay guys")
    print "Chat Message:"
    print GameProtocol.ChatMessage.unpack(bytestr)
    bytestr = GameProtocol.SpawnEnt("RobotJesus", 10, -15, 20)
    print "Spawn Message"
    print GameProtocol.read( bytestr)

if __name__=='__main__': test()

