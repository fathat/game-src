"""
Format characters:

  format   C Type               Python

    x       pad byte            no value
    c       char                string of length 1
    b       signed char         integer
    B       unsigned char       integer
    h       short               integer
    H       unsigned short      integer
    i       int                 integer
    I       unsigned int        long
    l       long                integer
    L       unsigned long       long
    q       long long           long    (1)
    Q       unsigned long long  long    (1)
    f       float               float
    d       double              float
    s       char[]              string
    p       char[]              string
    P       void *              integer
"""

## Field types that can be used in messages

class MsgField(object):
    #The number of active instances. We keep track of this
    #so that we can keep track of the order in which
    #each field is declared.
    instanceCount = 0

    formatString = ' '

    def __init__(self, always_first=False):
        if always_first:
            self.instanceNumber = 0
        else:
            self.instanceNumber = MsgField.instanceCount
            MsgField.instanceCount += 1

    def __repr__(self):
        return "<%s, instance number %s, format string %s>" % (
                 str(self.__class__.__name__),
                 str(self.instanceNumber),
                 str(self.formatString) )

class NumberField(MsgField):
    def __init__(self, unsigned=False, always_first=False):
        MsgField.__init__(self, always_first)
        if unsigned:
            self.formatString = self.formatString.upper()

class ByteField(NumberField):
    formatString = 'b'

class ShortField(NumberField):
    formatString = 'h'

class IntField(NumberField):
    formatString = 'i'

class LongField(NumberField):
    formatString = 'l'

class LongLongField(NumberField):
    formatString = 'q'

class FloatField(NumberField):
    formatString = 'f'

class DoubleField(NumberField):
    formatString = 'd'

class CStringField(MsgField):
    formatString = 's'
    def __init__(self, length):
        MsgField.__init__(self)
        self.length = length
        self.formatString = str(length) + self.formatString

class PascalStringField(MsgField):
    formatString = 'p'
    def __init__(self, length):
        MsgField.__init__(self)
        self.length = length
        self.formatString = str(length) + self.formatString

class PointerField(MsgField):
    formatString = 'P'

class MatrixField(MsgField):
    formatString = 'f'*16
