OBJS=vertexbuffer.cpp graphext_wrap.cxx
SRC=`echo $(OBJS) | sed 's/.o/.cpp/g'`
TARGET=_graphext.so
VERSION=0.0.1
LIBS= -lGL -lGLU
INCLUDEPATHS=-I/usr/include/python2.5
CC=g++
CFLAGS=-shared

all: graphext
	
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

.PHONY: mathhat clean depend

graphext: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(INCLUDEPATHS)
	
clean:
	rm -rf $(TARGET) *.o
	
depend:
	@makedepend $(CFLAGS) -Y $(SRC) 2> /dev/null
	
# DO NOT DELETE

udp.o: udp.h socket.h
tcp.o: tcp.h socket.h
tcpserver.o: tcpserver.h socket.h
packetizer.o: packetizer.h socket.h
