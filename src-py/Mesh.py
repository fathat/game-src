from OpenGL.GL import *
from cgkit.cgtypes import vec3, vec4
import glew
import array
import TextureManager as textureLoader
import os.path
import BoundingVolumes
import VertexBuffer
import ListFunctions
import Settings
import Graphics
from Graphics import GraphicsCard
from DisplayList import DisplayList
from ListFunctions import reinterpretFlatList

try:
    import Physics
except:
    Physics = None

class NotS3DFileException( Exception ):
    """Raised when the first 4 bytes of the file do not form the
       string S3D1"""
    pass

class S3DHeader(object):
    def __init__( self, fileobj ):
        #read the header data
        a = array.array('c')
        a.fromfile( fileobj, 4 )
        self.typeString = a.tostring()

        #print self.typeString
        if not self.typeString == 'S3D1':
            raise NotS3DFileException, 'Not a .S3D file'

        a = array.array( 'i' )
        a.fromfile( fileobj, 3 )
        self.meshCount     = a[0]
        self.materialCount = a[1]
        self.animationTime = a[2]


class S3DMeshInfo(object):
    vertexCount   = 0
    normalCount   = 0
    triangleCount = 0
    materialIndex = 0

    def __init__( self, fileobj ):
        a = array.array( 'i' )
        a.fromfile( fileobj, 4 )
        self.vertexCount   = a[0]
        self.normalCount   = a[1]
        self.triangleCount = a[2]
        self.materialIndex = a[3]


class S3DMaterial(object):

    def __init__(self, fileobject, mesh_path ):
        #read in color data
        a = array.array( 'f' )
        a.fromfile( fileobject, 18 )
        self.ambient  = a[0], a[1], a[2],  a[3]
        self.diffuse  = a[4], a[5], a[6],  a[7]
        self.specular = a[8], a[9], a[10], a[11]
        self.emissive = a[12], a[13], a[14], a[15]
        self.shininess    = a[16]
        self.transparency = a[17]

        #read in texture names
        a = array.array( 'c' )
        a.fromfile( fileobject, 256 )
        self.textureName = a.tostring()
        self.textureName = self.textureName[:self.textureName.find('\0')]

        a = array.array( 'c' )
        a.fromfile( fileobject, 256 )
        self.alphamapName = a.tostring()
        self.alphamapName = self.alphamapName[:self.alphamapName.find('\0')]

        #add relative path to texture path
        tn = mesh_path + '/' +  self.textureName

        #load textures
        self._textureName = tn#textureLoader.GetTexture( tn )
        """try:
            self.texture = textureLoader.GetTexture( tn )
        except Exception, e:
            print "[ERROR!]: Could not load mesh material " + tn
            print e
            self.texture = None"""

    @property
    def texture(self):
        return textureLoader.GetTexture( self._textureName )

    def cache(self):
        """Forces material to load its texture"""
        self.texture

    def __str__( self ):
        return ('ambient:  ' + str(self.ambient)
            + '\ndiffuse:  ' + str(self.diffuse)
            + '\nspecular: ' + str(self.specular)
            + '\nemissive: ' + str(self.emissive)
            + '\ntexture:  ' + self.textureName
            + '\nalphamap: ' + self.alphamapName)

    def setActive( self ):
        if self.texture:
            self.texture.bind(0)
        else:
            glDisable( GL_TEXTURE_2D )


class SubMesh(object):
    def __init__(self, raw_vertices, raw_normals, raw_triangles ):
        self.vertices  = reinterpretFlatList( raw_vertices, 5 )
        self.normals   = reinterpretFlatList( raw_normals, 3 )
        self.triangles = reinterpretFlatList( raw_triangles, 6 )
        self.polygonCount = len( self.triangles)
        self.material = None
        self.vertexbuffer = None
        self.indexbuffer = None
        self.makeArrays()
        self.calculateBoundingSphere()

    def __str__ (self ):
        return str(self.vertices) + str( self.triangles) + str( self.normals )

    def initDeviceResources(self):
        self.makeVertexBuffer()

    def getFaces( self ):
        """Returns faces as a list of lists (with each sublist having 9 floats
        to define the face) """
        faces = []
        for t in self.triangles:
            v1, v2, v3, n1, n2, n3 = t #unpack t
            v1 = self.vertices[v1]
            v2 = self.vertices[v2]
            v3 = self.vertices[v3]
            faces.append( [v1[0], v1[1], v1[2],
                       v2[0], v2[1], v2[2],
                       v3[0], v3[1], v3[2]] )
        return faces

    def makeVertexBuffer(self):
        self.vertexbuffer = VertexBuffer.MakeStandardVertexBuffer(
            vertices=[vec3(*x) for x in self.vertices_gl],
            normals=[vec3(*x) for x in self.normals_gl],
            tex_coords=ListFunctions.flattenListOfLists(self.texcoord_gl)
            )
        self.indexbuffer = VertexBuffer.IndexBufferObject( self.indices_gl )


    def makeArrays( self ):
        self.vertices_gl = []
        self.texcoord_gl = []
        #self.normals_gl  = []
        self.indices_gl  = []

        #Build vertices and texcoords
        for v in self.vertices:
            self.vertices_gl.append( [ v[0], v[1], v[2] ] )
            self.texcoord_gl.append( [ v[3], v[4] ] )

        #Build list of normals
        norms = self.normals
        self.normals_gl = range( len( self.vertices_gl ) )
        for t in self.triangles:
            ni0 = t[3]
            ni1 = t[4]
            ni2 = t[5]
#           self.normals_gl.append ( list( norms[ni0] ) )
#           self.normals_gl.append ( list( norms[ni1] ) )
#           self.normals_gl.append ( list( norms[ni2] ) )
            self.normals_gl[ t[0] ] = list( norms[ni0] )
            self.normals_gl[ t[1] ] = list( norms[ni1] )
            self.normals_gl[ t[2] ] = list( norms[ni2] )

            self.indices_gl.append (t[0] )
            self.indices_gl.append (t[1] )
            self.indices_gl.append (t[2] )

    def calculateBoundingSphere(self):
        self.boundingSphere = BoundingVolumes.createSphereAroundPoints(
                                [vec3(x[0], x[1], x[2]) for x in self.vertices_gl] )

    def visibleIn(self, camera, transform_matrix):
        return camera.sphereVisible( self.boundingSphere.transformedBy( transform_matrix)  )

    def drawArrays( self ):
        if not self.material == None:
            self.material.setActive()
        #glDisable( GL_TEXTURE_2D )
        glFrontFace( GL_CW )

        #set the vertex array pointer
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointerf(  self.vertices_gl )

        #set texcoord array for first texture stage
        glew.glClientActiveTextureARB( glew.GL_TEXTURE0_ARB )
        glTexCoordPointerf( self.texcoord_gl )
        glEnableClientState( GL_TEXTURE_COORD_ARRAY )

        #set the vertex array pointer
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointerf(  self.normals_gl )

        glDrawElementsui( GL_TRIANGLES, self.indices_gl )

        glDisableClientState( GL_NORMAL_ARRAY )
        glDisableClientState( GL_VERTEX_ARRAY )
        glDisableClientState( GL_TEXTURE_COORD_ARRAY )

    def startDraw(self):
        if not self.material == None:
            self.material.setActive()
        if self.vertexbuffer is None or self.indexbuffer is None:
            self.makeVertexBuffer()
        self.indexbuffer.bind()
        self.vertexbuffer.bind()

    def drawBuffer(self):
        self.indexbuffer.drawWithoutBinding()

    def endDraw(self):
        self.vertexbuffer.unbind()
        self.indexbuffer.unbind()

    def precacheDeviceResources(self):
        if self.material: self.material.cache()
        self.makeVertexBuffer()

    def drawVB(self):
        if not self.material == None:
            self.material.setActive()
        if self.vertexbuffer is None or self.indexbuffer is None:
            self.makeVertexBuffer()
        glFrontFace( GL_CW )
        self.vertexbuffer.bind()
        self.indexbuffer.draw()
        self.vertexbuffer.unbind()

    def draw( self ):
        if Settings.UseVBO:
            self.drawVB()
        else:
            self.drawSimple()
        #self.drawArrays()
        #self.drawSimple()

    def drawSimple (self):
        if not self.material == None:
            self.material.setActive()
        #glDisable( GL_TEXTURE_2D )
        glBegin(GL_TRIANGLES)

        for t in self.triangles:
            v1, v2, v3, n1, n2, n3 = t #unpack the triangle indices

            v1 = self.vertices[v1]
            v2 = self.vertices[v2]
            v3 = self.vertices[v3]

            n1 = self.normals[n1]
            n2 = self.normals[n2]
            n3 = self.normals[n3]

            glColor3f( 1.0, 1.0, 1.0 )

            glTexCoord2f( v1[3], v1[4] )
            glNormal3f( n1[0], n1[1], n1[2] )
            glVertex3f( v1[0], v1[1], v1[2] )

            glTexCoord2f( v2[3], v2[4] )
            glNormal3f( n2[0], n2[1], n2[2] )
            glVertex3f( v2[0], v2[1], v2[2] )

            glTexCoord2f( v3[3], v3[4] )
            glNormal3f( n3[0], n3[1], n3[2] )
            glVertex3f( v3[0], v3[1], v3[2] )

        glEnd()


class Mesh(object):

    def __init__( self, filename ):

        #open the file
        fileobj = open( filename, mode='rb' )
        self.filename = filename
        self.physicsMesh = None

        #read the header data
        header = S3DHeader( fileobj )

        #read the meshes
        self.meshes = []
        for i in range( header.meshCount ):
            m = S3DMeshInfo( fileobj )

            #read vertices (x, y, z, u, v )
            v = array.array( 'f' )
            v.fromfile( fileobj, 5*m.vertexCount )

            #read normals ( x, y, z )
            n = array.array( 'f' )
            n.fromfile( fileobj, 3*m.normalCount )

            #read triangles ( v1, v2, v3, n1, n2, n3 )
            t = array.array( 'i' )
            t.fromfile( fileobj, 6*m.triangleCount )

            sm = SubMesh( v.tolist(), n.tolist(), t.tolist() )
            sm.materialIndex = m.materialIndex
            self.meshes.append( sm )

        #extract the path from the filename
        file_path = os.path.dirname( filename )

        #read the materials
        self.materials = []
        for i in range( header.materialCount ):
            self.materials.append( S3DMaterial( fileobj, file_path ) )

        #associate the materials with the submeshes
        for m in self.meshes:
            if m.materialIndex != -1:
                m.material = self.materials[m.materialIndex]
            else:
                m.material = None

        fileobj.close()
        self.display_list = None
        self.polygonCount = self.getPolygonCount()
        self.calculateBoundingSphere()

    def precacheSubmeshes(self):
        """Creates display lists and loads textures for all the mesh's
        submeshes."""
        for m in self.meshes:
            m.precacheDeviceResources()

    def precacheDeviceResources(self):
        if self.display_list: return
        self.buildDisplayList()

    def buildDisplayList(self ):
        self.precacheSubmeshes()
        self.display_list = DisplayList( lambda: self.drawSimple() )

    def buildPhysicsMesh( self, solver ):
        if not Physics: return None
        self.physicsMesh = solver.makeMesh()

        physmesh_filename = os.path.splitext(self.filename)[0] + '.superserial'
        if os.path.exists( physmesh_filename ):
            self.physicsMesh.Deserialize( physmesh_filename )
            #print "Loaded serialized mesh..." + physmesh_filename
        else:
            self.physicsMesh.BeginBuild()
            for m in self.meshes:
                faces = m.getFaces()
                for f in faces:
                    self.physicsMesh.AddFace( f, 0 )
            self.physicsMesh.EndBuild( 1 )
            self.physicsMesh.Serialize( physmesh_filename )
            print "Could not find serialized mesh, regenerating.. " + physmesh_filename
        return self.physicsMesh

    def getPhysicsMesh( self, solver ):
        if not self.physicsMesh:
            self.buildPhysicsMesh( solver )
        return self.physicsMesh

    def getPolygonCount(self):
        i = 0
        for m in self.meshes:
            i += m.polygonCount
        return i

    def allVertices(self):
        vertices = []
        for m in self.meshes:
            vertices.extend( m.vertices_gl )
        return vertices

    def calculateBoundingSphere(self):
        #self.boundingSphere = BoundingVolumes.createSphereAroundSpheres( [x.boundingSphere for x in self.meshes ] )
        self.boundingSphere = BoundingVolumes.createSphereAroundPoints(
                                    [vec3(x[0], x[1], x[2]) for x in self.allVertices()] )


    def visibleIn(self, camera, transform_matrix):
        return camera.sphereVisible( self.boundingSphere.transformedBy( transform_matrix)  )

    def boundingSphereAt( self, transform_matrix):
        return self.boundingSphere.transformedBy( transform_matrix)

    def draw(self):
        glFrontFace( GL_CW )
        if Settings.UseDisplayLists:
            if self.display_list is None: self.buildDisplayList()
            self.display_list()
        else:
            self.drawSimple()
        GraphicsCard.recordDraw(self.polygonCount)

    def drawSimple ( self ):
        for m in self.meshes:
            m.draw()

    def drawBoundingSphere(self):
        self.boundingSphere.draw()


if __name__ == '__main__':
    Mesh( 'crate.s3d' )
