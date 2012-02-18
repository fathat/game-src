#!/usr/bin/env python
import sys
import threading
from Client import Client
from GameProtocol import GameProtocol

name = ""
class ChatClient(Client):

    def onConnect(self, event):
        print "Connected to:", event.peer.address
        self.send( GameProtocol.SetName(name) )

    def onDisconnect(self, event):
        print "Closed connection to:", event.peer.address

    def onReceive(self, event):
        print "Received Message", GameProtocol.read( event.packet.data )

    def runUpdateThread(self):
        class NetThread(threading.Thread):
            def run(*args):
                while self.state != 'disconnected':
                    self.update()
        self.updateThread = NetThread()
        self.updateThread.start()

    def stopUpdateThread(self, timeout=2.0):
        self.updateThread.join(timeout)


def simpletest():
    global name
    print "Name?"
    name = sys.stdin.readline(32)
    ch = ChatClient( '127.0.0.1', 1234 )
    ch.runUpdateThread()
    try:
        done = False
        while not done:
            input = sys.stdin.readline()
            if input.strip() == 'quit':
                ch.disconnect()
                done = True
            else:
                ch.send(GameProtocol.ChatMessage(input))
    except KeyboardInterrupt:
        print "Quitting..."
        ch.disconnect()
        done = True
    ch.stopUpdateThread()

if __name__== '__main__':
    simpletest()


