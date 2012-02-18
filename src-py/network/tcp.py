import socket
import time

class TCPSocket:
	'''Sends and receives datagrams.'''
	def __init__(self, sock=None):
		if sock is None:
			self.sock = socket.socket(
				socket.AF_INET, socket.SOCK_STREAM)
			self.sock.settimeout( 4.0 )
		else:
			self.sock = sock

	def connect(self, host, port):
		self.sock.connect((host, port))
		self.sock.setblocking(true)

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
