#!/usr/bin/env python
import sys
import threading
from Server import Server
from GameProtocol import GameProtocol

class ChatServer(Server):

    def onConnect(self, event):
        print event.peer.address, "connected."

    def onDisconnect(self, event):
        print "Closed connection to:", event.peer.address

    def onReceive(self, event):
        msg = GameProtocol.read( event.packet.data )
        if 'message' in msg.keys():
            self.sendToAll( GameProtocol.ChatMessage(msg['message']) )
        print event.peer.address, msg

def simpletest():
    ch = ChatServer( 1234, 32 )
    try:
        while True:
            ch.update()
    except KeyboardInterrupt:
        print "Quitting..."

if __name__== '__main__':
    simpletest()
