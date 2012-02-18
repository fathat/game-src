import enet
from Connection import Connection
import Protocol

class Client(object):
    def __init__(self, addr, port, channels=2):
        self.enetHost = enet.Host( None, 1, 0, 0 )
        if self.enetHost is None: raise "Could not create host"
        self.connection = None
        self.state = "connecting"
        self.enetHost.connect( enet.Address( addr, port), channels )

    def disconnect(self):
        if self.connection:
            self.connection.close()
            del self.connection
            self.connection = None

    def _onConnectEvent(self, event):
        self.state = "connected"
        self.connection = Connection(event)
        self.onConnect(event)

    def _onDisconnectEvent(self, event):
        self.state = "disconnected"
        event.peer.disconnect(0)
        self.onDisconnect(event)

    def _onReceiveEvent(self, event):
        self.onReceive(event)

    @property
    def isConnected(self): return self.state == "connected"

    def onConnect(self, event): pass
    def onDisconnect(self, event): pass
    def onReceive( self, event): pass

    def send(self, data_string, reliable=True):
        if reliable:
            flag = enet.PACKET_FLAG_RELIABLE
        else:
            flag = None
        self.enetHost.broadcast(0, enet.Packet( data_string, flag ) )

    def update(self):
        while True:
            event = self.enetHost.service(0)
            if not event:
                return
            if event.type == enet.EVENT_TYPE_CONNECT:
                self._onConnectEvent(event)
            elif event.type == enet.EVENT_TYPE_DISCONNECT:
                self._onDisconnectEvent(event)
            elif event.type == enet.EVENT_TYPE_RECEIVE:
                self._onReceiveEvent( event)
            elif event.type == enet.EVENT_TYPE_NONE:
                return

def simpletest():
    ch = Client( '127.0.0.1', 1234 )
    try:
        while True: ch.update()
    except KeyboardInterrupt:
        print "Quitting..."

if __name__=='__main__':
    simpletest()
