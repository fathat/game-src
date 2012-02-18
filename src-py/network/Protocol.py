from Message import CreateMsgType, ReadID, MessagePacket, MetaPacket

class MetaProtocol(type):
    def __new__(meta, classname, bases, classDict):
        #grab all the message types, and create a dictionary
        #to find them by id number
        all_message_types = [t for n,t in classDict.items()
                                if type(t) is MetaPacket]
        lookup = {}
        for t in all_message_types:
            lookup[t.typeNum] = t
        classDict['lookup'] = lookup
        return type.__new__(meta, classname, bases, classDict)

class NetworkProtocol(object):
    __metaclass__ = MetaProtocol
    
    @classmethod
    def read(cls, bytestr):
        msgclass = cls.lookup[ReadID(bytestr)]
        msg = msgclass()
        msg.decode(bytestr)
        return msg

class Protocol(object):

    def __init__(self, *args):
        self.idLookup = {}
        for x in args:
            msg = CreateMsgType(*x)
            setattr(self, msg.name, msg)
            self.idLookup[msg.id] = msg

    def read(self, bytestr):
        id = ReadID(bytestr)
        return self.idLookup[id].unpackAsObject(bytestr)


