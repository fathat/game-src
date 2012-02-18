from network.Client import Client
from GameProtocol import GameProtocol

class GameClient(Client):
    channels = 64

    def __init__(self, addr, port, world, name ):
        self.screenName = name
        self.world = world
        Client.__init__(self, addr, port, self.channels)

    def onConnect(self, event):
        print "Connected to:", event.peer.address
        self.send( str(GameProtocol.SetName(self.screenName)) )

    def onDisconnect(self, event):
        print "Closed connection to:", event.peer.address

    def onReceive(self, event):
        msg = GameProtocol.read( event.packet.data )
        if msg.name == 'SpawnEnt':
            #print msg
            self.world.netSpawnEnt( msg )
        elif msg.name == 'MoveEnt':
            #print event.packet
            self.world.netMoveEnt( msg )
        elif msg.name == 'MoveActor':
            self.world.netMoveActor(msg)
