import enet
from Platform import time_in_seconds


class Connection(object):
    def __init__(self, connect_event):
        self.address = connect_event.peer.address
        self.peer    = connect_event.peer
        self.active = True
        print "Connect Event"
        print dir(connect_event)

    def close(self):
        self.active = False
        self.peer.disconnect()

    def send(self, data, channel=0, reliable=True, unsequenced=False):
        flags = 0
        if reliable:
            flags |= enet.PACKET_FLAG_RELIABLE
        if unsequenced:
            flags |= enet.PACKET_FLAG_UNSEQUENCED
        self.peer.send(channel, enet.Packet(data, flags))

    @property
    def ping(self):
        return self.peer.roundTripTime

    def isAtAddress(self, address):
        return (address.port == self.address.port
                and address.host == self.address.host)

    @property
    def state(self):
        state_d = {enet.PEER_STATE_DISCONNECT  : 'disconnected',
                   enet.PEER_STATE_CONNECTING  : 'connecting',
                   enet.PEER_STATE_CONNECTED   : 'connected',
                   enet.PEER_STATE_DISCONNECTING : 'disconnecting',
                   enet.PEER_STATE_ACKNOWLEDGING_CONNECT : 'ack_connect',
                   enet.PEER_STATE_ACKNOWLEDGING_DISCONNECT:'ack_disconnect',
                   enet.PEER_STATE_ZOMBIE : 'zombie' }
        return state_d[self.peer.state]

    @property
    def inbox(self):
        """Returns a list of packets or None"""

    def nextMessage(self):
        """returns then next message in the queue"""
