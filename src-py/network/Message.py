import struct
from Fields import *

#Utility functions
def StripZeroesFromString( aString ):
    return ''.join( [x for x in aString if x != '\x00' ] )

def FormatStringForFields( fields):
    return ''.join( [ f[1] for f in fields] )

def ReadID(data_string):
    return struct.unpack('B', data_string[:1])[0]

_id = 0
def NewID():
    global _id
    _id += 1
    return _id

class MetaPacket(type):
    """A metaclass for packets. Mostly just initializes a few
    class side variables (sortedFields and typeNum."""
    def __new__(meta, classname, bases, classDict):
        #parse out all the MsgFields, sort them, and
        #put them in the new class
        classDict['id'] = ByteField(unsigned=True, always_first=True)
        fields = [(x[0], x[1]) for x in classDict.items()
                    if isinstance(x[1], MsgField) ]
        fields.sort(lambda x,y: x[1].instanceNumber - y[1].instanceNumber)
        classDict['formatString'] = ''.join([f[1].formatString for f in fields])
        classDict['sortedFields'] = [f[0] for f in fields]
        classDict['typeNum'] = NewID()
        return type.__new__(meta, classname, bases, classDict)

class MessagePacket(object):
    __metaclass__ = MetaPacket
    
    
    def __init__(self, *args, **kwargs):
        #fill in ID field
        self.id = self.typeNum
        
        #start at field 1, since field 0 is ID (which is already filled in)
        for i in xrange(len(args)):
            fieldname = self.sortedFields[i+1]
            setattr(self, fieldname, args[i])
        for key, val in kwargs:
            setattr(self, key, val)
    
    def __str__(self):
        return self.asByteString()
    
    def __repr__(self):
        repstr = "%s {\n" % (self.__class__.__name__)
        for k in self.sortedFields:
            repstr += "  %s: %s\n" % ( str(k), repr(getattr(self, k) ) )
        repstr += "}"
        return repstr
    
    @property
    def name(self):
        return self.__class__.__name__
    
    def asByteString(self):
        vals = [getattr(self, field_name) for field_name in self.sortedFields]
        return struct.pack( self.formatString, *vals)
    
    def decode(self, bytestring):
        vals = struct.unpack(self.formatString, bytestring)
        for i in xrange(len(self.sortedFields)):
            field = self.sortedFields[i]
            val = vals[i]
            if isinstance( val, str ):
                val = StripZeroesFromString(val)
            setattr(self, field, val)
#        for field, val in zip(self.sortedFields, vals):
#            if isinstance( val, str ):
#                val = StripZeroesFromString(val)
#            setattr(self, field, val)

    def printFields(self):
        for x in self.sortedFields:
            print x

class ExamplePacketClass(MessagePacket):
    id = ByteField()
    c  = CStringField(length=255)
    b  = DoubleField()
    a  = IntField(unsigned=True)
 

class MessageStruct(object):
    def __init__(self, d):
        for k, v in d.items():
            setattr(self, k, v)

    def __repr__(self):
        repstr = "%s {\n" % (self.name)
        for k,v in self.__dict__.items():
            repstr += "  %s: %s\n" % ( str(k), repr(v) )
        repstr += "}"
        return repstr

class MessageType(object):
    def __init__(self, name, id, *args):
        self.name = name
        self.id = id
        self.encoder = None
        #print "ID", self.id
        self.fields = (('id', 'B'),) + args[:]
        #print "Creating message with fields:", self.fields
        self.fieldTypeLookup = {}
        for f in self.fields:
            n, t = f
            self.fieldTypeLookup[n] = t
        self.formatString = FormatStringForFields(self.fields)


    def _isFieldString(self, fieldname):
        return self.fieldTypeLookup[fieldname].endswith('s')

    def __call__(self, *args):
        return self.makeByteString(*args)

    def makeByteString(self, *args):
        params = (self.id,) + args
        return struct.pack( self.formatString, *params)

    def unpackAsDictionary(self, bytestring):
        names = [x for x,y in self.fields]
        vals = struct.unpack(self.formatString, bytestring)
        d = {'name' : self.name}
        for name, value in zip(names, vals):
            if self._isFieldString( name ):
                d[name] = StripZeroesFromString( value )
            else:
                d[name] = value
        return d

    def unpackAsObject(self, bytestring):
        return MessageStruct( self.unpackAsDictionary(bytestring) )


    def __repr__(self):
        vars = [(a,v) for a,v in self.__dict__.items()
                if a.startswith('__') == False]
        return str(vars)


def CreateMsgType(*args): return MessageType(*args)

def test():
    fields = (('a', 'b'), ('lol', '255s'), ('b', 'f'))
    format_string = FormatStringForFields( fields )
    data_string = struct.pack(format_string, 1, 'HAY GUYS', -100 )
    id = struct.unpack('i', data_string[:1])
    print id

