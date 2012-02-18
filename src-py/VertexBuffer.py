import array
import graphext
import Graphics
from Graphics import GraphicsCard
from DisplayList import DisplayList
from OpenGL.GL import *
from glew import glBindBufferARB, glGenBuffersARB, glDeleteBuffersARB, glBufferDataARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB, glClientActiveTextureARB, GL_TEXTURE0_ARB

#Mappings to convert an array module type string to
#an opengl type enumeration
array_type_to_gl_type = {
    'f' : GL_FLOAT,
    'B' : GL_UNSIGNED_BYTE,
    'h' : GL_SHORT,
    'H' : GL_UNSIGNED_SHORT,
    'i' : GL_INT,
    'I' : GL_UNSIGNED_INT,
    'd' : GL_DOUBLE
}

element_type_to_function_mapping = {
    'vertex'   : (graphext.VertexOffset, GL_VERTEX_ARRAY),
    'normal'   : (graphext.NormalOffset, GL_NORMAL_ARRAY),
    'color'    : (graphext.ColorOffset, GL_COLOR_ARRAY),
    'texcoord' : (graphext.TextureOffset, GL_TEXTURE_COORD_ARRAY)
}

id_counter = 0

def GenerateID():
    global id_counter
    id_counter += 1
    return id_counter

class DataBlock(object):
    """A block of data that will get stored as a string"""

    def __init__(self, element_type='vertex', data=[], array_type='f', elements=3, tex_coord_index=None):
        """Initializes the data block"""
        self.elementsType = element_type
        self.arrayType = array_type
        self.glType = array_type_to_gl_type[array_type]
        self.data = array.array( array_type, data )
        self.datastring = self.data.tostring()
        self.sizeInBytes = len(self.datastring)
        self.elements = elements
        self.offset = 0
        f, client_state = element_type_to_function_mapping[element_type]
        self.pointerFunction = f
        self.clientState = client_state
        self.texCoordIndex = tex_coord_index


    def bind(self):
        """Set the pointer to the proper place in the VBO"""
        if self.texCoordIndex != None:
            glClientActiveTextureARB( GL_TEXTURE0_ARB + self.texCoordIndex )
        glEnableClientState( self.clientState )
        self.pointerFunction( self.elements, self.glType, self.offset )



def MakeDataBlockFromVec3( vecs, element_type ):
    float_array = []
    for v in vecs: float_array.extend( [ v.x, v.y, v.z ] )
    return DataBlock( element_type, float_array, 'f', 3 )


def MakeDataBlockFromVec4( vecs, element_type ):
    float_array = []
    for v in vecs: float_array.extend( [ v.x, v.y, v.z, v.w ] )
    return DataBlock( element_type, float_array, 'f', 4  )

def MakeDataBlockFromFloats( float_list, nums_per_element, element_type, tex_index=None ):
    return DataBlock( element_type, float_list, 'f', nums_per_element, tex_index )


class VertexBufferObject(object):
    def __init__(self, datablocks, usage=GL_STATIC_DRAW_ARB ):
        """Initializes the vertex buffer object with the given data blocks"""
        DisplayList.checkForInvalidCall("Should not construct VertexBuffers in a display list")
        self.dataBlocks = datablocks[:]
        self.totalSize = 0
        self.usage = usage
        self.id = GenerateID()
        self._upload()

    #def __del__(self):
    #   glDeleteBuffersARB(1, [self.id])
    #   pass

    def _upload(self):
        """Copies all the vertex data to the graphics card"""
        #determine the offsets for each block and the total size
        offset = 0
        for block in self.dataBlocks:
            block.offset = offset
            offset += block.sizeInBytes
        self.totalSize = offset

        #create one monolithic data string and give it to opengl
        self.datastring = ''.join( [ x.datastring for x in self.dataBlocks ])
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, self.id )
        glBufferDataARB( GL_ARRAY_BUFFER_ARB,len(self.datastring), self.datastring, self.usage )
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0)

    def bind(self):
        """Binds the buffer for OpenGL"""
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, self.id )
        for block in self.dataBlocks: block.bind()

    def unbind(self):
        """Unbinds the buffer for OpenGL"""
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0 )

class IndexBufferObject(object):
    def __init__(self, indices, usage=GL_STATIC_DRAW_ARB, element_type='H'):
        DisplayList.checkForInvalidCall("Should not construct IndexBuffers in a display list")
        self.id = GenerateID()
        self.usage = usage
        self.indices = indices
        self.elementType = element_type
        self.glTypeCode = array_type_to_gl_type[element_type]
        self._loaddata()

    def _loaddata(self):
        a = array.array(self.elementType, self.indices)
        self.datastring = a.tostring()
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, self.id )
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, len(self.datastring), self.datastring, self.usage )
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0 )

    def bind(self):
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, self.id )

    def drawWithoutBinding(self, mode=GL_TRIANGLES, count=None, offset=0):
        if count == None:
            count = len(self.indices)
        graphext.DrawElementsVB( mode, count, self.glTypeCode, 0 )
        GraphicsCard.recordDraw( int(count/3))

    def draw(self, mode=GL_TRIANGLES, count=None, offset=0):
        #glDrawElements( GL_TRIANGLES, 1, self.glTypeCode, 0 )
        if count == None:
            count = len(self.indices)
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, self.id )
        graphext.DrawElementsVB( mode, count, self.glTypeCode, 0 )
        GraphicsCard.recordDraw( int(count/3))
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0 )

    def unbind(self):
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0 )


def MakeStandardVertexBuffer(vertices=None, normals=None, colors=None,
                             tex_coords=None, tex_coords_2=None, tex_coords_3=None):
    datablocks = []
    if vertices: datablocks.append( MakeDataBlockFromVec3( vertices, 'vertex' ) )
    if normals:  datablocks.append( MakeDataBlockFromVec3( normals, 'normal' ) )
    if colors:   datablocks.append( MakeDataBlockFromVec4( colors, 'color' ) )
    if tex_coords:   datablocks.append( MakeDataBlockFromFloats( tex_coords, 2, 'texcoord', 0 ) )
    if tex_coords_2:   datablocks.append( MakeDataBlockFromFloats( tex_coords_2, 2, 'texcoord', 1 ) )
    if tex_coords_3:   datablocks.append( MakeDataBlockFromFloats( tex_coords_3, 2, 'texcoord', 2 ) )
    return VertexBufferObject( datablocks )


