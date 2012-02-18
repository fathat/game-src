import Settings
import World

from Platform import time_in_seconds, asUnixPath
from network.Server import Server
from GameProtocol import GameProtocol

class Player(object):
    pass

class GameServer(Server):
    def __init__(self, mod, map, port, max_connections):
        Server.__init__(self, port, max_connections)
        Settings.RunAI = True
        self.world = World.World(Settings.DefaultMod,
                                 Settings.DefaultMap,
                                 is_server=True,
                                 graphics_enabled=False)
        print "Server running, listening for messages..."

    def onConnect(self, connection, event):
        print "Accepted connection from:", event.peer.address

        #construct spawn ent messages
        print "making spawn packets"
        def makepacket(ent):
            assert ent.entID is not None
            return str(GameProtocol.SpawnEnt(
                                         asUnixPath(ent.typeInfo.typePath),
                                         ent.scriptname,
                                         ent.entID,
                                         ent.location.x,
                                         ent.location.y,
                                         ent.location.z,
                                         ent.q4.x,
                                         ent.q4.y,
                                         ent.q4.z,
                                         ent.q4.w))
        packets = [ makepacket(x) for x in self.world.scene.dynamicEnts
                    if x.serverside ]

        print "made packets"
        #send out the packets
        for packet in packets:
            connection.send( packet )
        connection.lastMoveUpdate = time_in_seconds()
        print "packets sent"

    def onDisconnect(self, connection, event):
        print "Closed connection to:", event.peer.address
        #delete the connection
        connections_to_close = [x for x in self.connections.items()
                                if x[1].state == 'disconnected']
        for addr, con in connections_to_close:
            print "Deleting connection", self.connections[addr]
            del self.connections[addr]

    def onReceive(self, connection, event):
        #relay
        print "Received message on connection:", connection
        msg = GameProtocol.read( event.packet.data )
        print msg

    def sendMovePackets(self):
        packets = self.world.makeMovePackets()
        inv_move_rate = 1.0/Settings.NetMovePacketSendRate

        for p in packets:
            self.sendToAll(p, 1, False, True)
            self.host.flush()
        #cons = [c for c in self.connections.values()
        #        if time_in_seconds() - c.lastMoveUpdate > inv_move_rate ]
        #chan = 1
        #for con in cons.values():
        #    for p in packets:
        #        con.send(p, chan, reliable=False, unsequenced=True)
        #        self.host.flush()
        #    con.lastMoveUpdate = time_in_seconds()

    def update(self, timedelta):
        Server.update(self)
        if len(self.connections):
            self.world.update(timedelta)
            self.sendMovePackets()

def main():
    last_fps_write = time_in_seconds()
    server = GameServer( Settings.DefaultMod, Settings.DefaultMap, 1337, 32)
    done = False
    time_delta = 0.001 #just assume 1 millisecond for the first frame
    while not done:
        last_tick  = time_in_seconds()
        server.update(time_delta)
        time_delta = time_in_seconds() - last_tick
        if time_in_seconds() - last_fps_write > 2:
            print 'FPS:', 1.0/time_delta
            last_fps_write = time_in_seconds()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print "Server shutdown"
