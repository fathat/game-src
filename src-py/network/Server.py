#from enet import *
import enet
from Connection import Connection
import Protocol


class Server(object):
    """YOU GOT SERVED! NOW ITS ON! <('-'<) <('-')> (>'-')>"""

    def __init__(self, port, max_connections):
        self.address = enet.Address( enet.HOST_ANY, port )
        self.maxConnections = max_connections
        self.host = enet.Host( self.address, max_connections, 0, 0)
        if not self.host:
            raise "Host could not be created"

        #connections stored in a dictionary, with the keys being
        #(host_string, port_number) tuple.
        self.connections = {}

    def _onConnectEvent(self, event):
        """Handle connect event"""
        con = Connection(event)
        self.connections[(event.peer.address.host, event.peer.address.port)] = con
        self.onConnect( con, event)

    def _onDisconnectEvent(self, event):
        """Handle disconnect event"""
        #only keep connections that don't match the disconnected
        #connections address
        self.onDisconnect( None, event )

    def _onReceiveEvent(self, event):
        """Handle message received event"""
        self.onReceive( self._findConnectionForEvent(event), event)

    def onConnect(self, connection, event): pass
    def onDisconnect(self, event, connection): pass
    def onReceive( self, event, connection): pass

    def _findConnectionForEvent(self, event):
        connection = self.connections[(event.peer.address.host,
                                       event.peer.address.port)]
        assert connection.isAtAddress(event.peer.address)
        return connection

    def sendToAll(self, data, channel=0, reliable=True, unsequenced=False):
        assert isinstance(data, str)
        flag = 0
        if reliable:
            flag = enet.PACKET_FLAG_RELIABLE
        if unsequenced:
            flag = enet.PACKET_FLAG_UNSEQUENCED

        self.host.broadcast(channel, enet.Packet(data, flag))

    def update(self):
        event = self.host.service(1)
        if not event:
            pass
        elif event.type == enet.EVENT_TYPE_CONNECT:
            self._onConnectEvent(event)
        elif event.type == enet.EVENT_TYPE_DISCONNECT:
            self._onDisconnectEvent(event)
        elif event.type == enet.EVENT_TYPE_RECEIVE:
            self._onReceiveEvent( event)
        elif event.type == enet.EVENT_TYPE_NONE:
            return

def main():
    server = Server(1234, 32)
    while True:
        server.update()


if __name__ == '__main__':
    main()
