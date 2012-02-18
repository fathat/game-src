import socket
import time

class UDPSocket:
    '''Sends and receives datagrams.'''
    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.settimeout( 4.0 )
        else:
            self.sock = sock
    def connect(self, host, port):
        self.sock.connect((host, port))
    def send(self, msg):
        sent = self.sock.send(msg)
        if sent == 0:
            raise RuntimeError, "socket connection broken"
    def receive(self ):
        msg = ''

        try:
            msg = self.sock.recv(8192)
        except:
            print "Could not receive from server"
            self.close()

        return msg
    def close(self):
        self.sock.close();

def parseGamespyInfo( infoString ):
    tokens = infoString.split("\\" );
    valueList = []
    for i in range( 1, len(tokens)-1, 2 ):
        valueList.append( (tokens[i].strip(), tokens[i+1].strip()) )
        #print tokens[i].strip(), ":", tokens[i+1].strip()
    return valueList

def rawQuery( hostname, port, queryString ):
    sock = UDPSocket()
    sock.connect( hostname, port )
    startTime = time.time()
    sock.send( queryString )
    info = sock.receive()
    sock.close()
    return info

def queryServerInfo( hostname, port, queryString ):
    return parseGamespyInfo( rawQuery( hostname, port, queryString) )




