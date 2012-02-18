import os
import xml.dom.minidom
from OpenGL.GL import *
from glew import *
from cgkit.cgtypes import *
import Shader
import TextureManager
import string
import ListFunctions
import Physics
import Graphics
import Grass
import Settings
import VertexBuffer
import ListFunctions
import MathUtil
import AStar
from Graphics import GraphicsCard
from DisplayList import DisplayList

class TerrainPatch(object):

    def __init__(self, screen, world, heightmap, material, spacing  ):
        """Initialize the terrain. Note that size should be 2^n+1"""
        self.screen    = screen
        self.world     = world
        self.heightmap = heightmap
        self.size      = heightmap.width
        self.textureRepeatRate = 0.50
        self.material  = material
        self.spacing   = float(spacing)
        self.texture   = None
        self.vertexArray = self.makeVertexArray()
        self.texCoords   = self.makeTexCoordArray()
        self.worldTexCoords = self.makeWorldTexCoordArray()
        self.indexArray     = self.makeIndexArray()
        self._markWalkableAreas()
        self.displayList = None
        #Create the physics body for collision
        self._createPhysicsBody()

    def initGraphics(self):
        if Settings.UseVBO:
            print "Creating Terrain Vertex Buffer..."
            self._createVertexBuffer()
        else:
            print "Not creating vertex buffer"

        if Settings.DrawGrass:
            self.grassPatch = Grass.GrassPatch(self.world,
                                               0,
                                               self,
                                               1000000, 4, 8,
                                               "../base/art/land/grass_side.png" )
        print "Building terrain display list..."
        if not Settings.SimpleTerrain:
            self._buildHighDetailDisplayList()
        else:
            self._buildLowDetailDisplayList()

    def _createVertexBuffer(self):
        self.vertexbuffer = VertexBuffer.MakeStandardVertexBuffer(
            vertices=[vec3(*x) for x in self.vertexArray],
            tex_coords=ListFunctions.flattenListOfLists( self.texCoords ),
            tex_coords_2=ListFunctions.flattenListOfLists( self.worldTexCoords )
            )
        self.indexbuffer = VertexBuffer.IndexBufferObject( self.indexArray, element_type='I' )

    def _createPhysicsBody( self):
        """Creates a physics body"""
        solver = self.world.solver

        #Create the collision geometry
        print "Creating collision geometry..."
        self.collisionGeom = self.world.solver.makeMesh( )

        #check if the heightmap mesh has already been serialized..
        meshpath = self.world.fsPath() + 'terrain.superserial'
        if os.path.isfile( meshpath ):
            #...if it has, just load it
            print "Heightmap mesh collision exists, loading.."
            self.collisionGeom.Deserialize( meshpath )
        else:
            self.collisionGeom.BeginBuild()
            faces = self.getFaceList()
            for face in faces:
                self.collisionGeom.AddFace( ListFunctions.flattenListOfLists( face), 0 )
            self.collisionGeom.EndBuild(0)
            print "Heightmap mesh collision built, saving for future uses..."
            self.collisionGeom.Serialize( meshpath )

        #Create the body
        self.body = solver.makeBody( self.collisionGeom, self )
        self.body.TreeCollisionCallback = self.onHeightmapCollision

    def _markWalkableAreas(self):
        self.walkableMask   = [True]*self.size*self.size
        index = 0
        for tz in xrange(self.size):
            for tx in xrange(self.size):
                v1,v2,v3,v4,v5,v6 = self.heightmap.getTrianglesAt( tx, tz )
                normal1 = MathUtil.getTriangleNormal( v1, v3, v2  )
                normal2 = MathUtil.getTriangleNormal( v4, v6, v5  )

                #mix the two normals to get an average
                normal = ((normal1 + normal2)/2.0).normalize()
                height = self.heightmap.getHeight(tx, tz)
                if height < self.world.seaLevel or normal.y < 0.1:
                    self.walkableMask[index] = False
                index += 1
        self.astarMap = AStar.AStarMap( self.walkableMask, self.size, self.size )

    def isAreaPassable(self, wx, wz ):
        tx = int(wx/self.spacing)
        tz = int(-wz/self.spacing)
        return self.walkableMask[ tx + tz * self.size ]

    def path(self, start, end):
        """start and end should be world coordinates (Stored as vec3).
        Path is returned as a list of vec3's. Note that the Y coordinate
        of these vec3's will always be zero (not the height of the terrain
        at that point)"""
        sx, sz = start.x / self.spacing, start.z / self.spacing
        dx, dz = end.x / self.spacing, end.z / self.spacing
        blockpath = self.astarMap.path( int(sx), int(sz), int(dx), int(dz) )

        #convert blockpath into world coordinates
        # [ vec3((c[0]+0.5)*self.spacing, 0, -(c[1]+0.5)*self.spacing)  for c in blockpath]
        returnlist = []
        for c in blockpath:
            wx = (c[0] + 0.5)*self.spacing
            wz = (c[1] + 0.5)*self.spacing
            wy = self.getWorldHeightValue( wx, wz )
            returnlist.append( vec3( wx, wy, wz ) )
        return returnlist


    def onHeightmapCollision( self, body ):
        pass

    def getFaceList( self ):
        """Gets the face list"""
        faces = []
        for i in range( 0, len(self.indexArray), 3 ):
            v0 = self.vertexArray[ self.indexArray[i+0 ] ]
            v1 = self.vertexArray[ self.indexArray[i+1 ] ]
            v2 = self.vertexArray[ self.indexArray[i+2 ] ]
            faces.append( [ v0, v2, v1 ] )
        return faces

    def getextent( self ):
        return self.size * self.spacing
    extent = property(getextent)

    def calcFlatNormals(self):
        pass

    def calcNormals( self ):
        pass

    def getWorldHeightValue( self, x, z ):
        return self.heightmap.getInterpolatedHeight( x / self.spacing, abs(z / self.spacing) )

    def getHeightValue( self, x, z ):
        return self.heightmap.getInterpolatedHeight( x, abs(z))

    def makeVertexArray ( self ):
        va = []
        h = self.heightmap.heightData
        for z in range( self.size ):
            for x in range( self.size ):
                va.append( [ float(x * self.spacing),
                             float(h[x+z*self.size]) ,
                             float(z * self.spacing) ])
        return va

    def makeTexCoordArray( self ):
        tc = []
        for z in range( self.size ):
            for x in range( self.size ):
                tc.append( [float(x) * self.textureRepeatRate, float(z)*self.textureRepeatRate ] )
        return tc

    def makeWorldTexCoordArray( self ):
        tc = []
        for z in range( self.size ):
            for x in range( self.size ):
                tc.append( [ float(x) / self.size, float(z) / self.size ])
        return tc


    def makeQuadIndexArray(self):
        ia = []
        for z in range( self.size-1 ):
            for x in range( self.size-1 ):
                ia.append( x + z * self.size )
                ia.append( x + (z+1)*self.size )
                ia.append( (x+1) + (z+1)*self.size )
                ia.append( (x+1) + z*self.size )

        return ia

    def makeTriangleIndexArray( self ):

        #NOTE: Don't change the vertex order or it will break newton's
        #collision detection!
        ia = []
        for z in range( self.size-1 ):
            for x in range( self.size-1 ):
                ia.append( x + z * self.size )
                ia.append( (x+1) + z*self.size )
                ia.append( x + (z+1)*self.size )

                ia.append( x + (z+1)*self.size )
                ia.append( (x+1) + z*self.size )
                ia.append( (x+1) + (z+1)*self.size )
        return ia

    def makeIndexArray(self):
        return self.makeTriangleIndexArray()

    def _buildPrimCL( self ):
        self.primDL = DisplayList( lambda: self.drawPrim() )
        print "Built primitive display list"

    def _buildLowDetailDisplayList(self):
        #make sure all the textures are loaded BEFORE we create the display
        #list.
        self.material.precacheGraphics()
        def _commands():
            self.material.start()
            self.drawSinglePass()
            self.material.end()
        self.displayList = DisplayList( _commands)

    def _buildHighDetailDisplayList( self ):
        if Settings.DrawGrass:
            self.buildGrassDL()

        #make sure all the textures are loaded BEFORE we create the display
        #list.
        self.material.precacheGraphics()
        self._buildPrimCL()

        print "Constructing main display list..."
        def _commands():
            #set some initial states
            glEnable( GL_TEXTURE_2D )
            glDisable( GL_LIGHTING )
            glDepthFunc( GL_LEQUAL )

            #Load in the material, and draw each layer
            mat = self.material
            mat.start()
            print "Material bound..."
            i = 0
            if Settings.UseVBO:
                print "Binding vertex buffer"
                self.vertexbuffer.bind()
                print "Binding index buffer"
                self.indexbuffer.bind()
            glFrontFace(GL_CCW)
            for l in mat.layers:
                l.use()
                #Graphics.setTextureAnisotropy( 8.0 )
                #print Graphics.getTextureAnisotropy()
                if Settings.UseVBO:
                    self.indexbuffer.drawWithoutBinding()
                else:
                    self.primDL()
                #self.drawVertexArrays()
                i += 1
            if Settings.UseVBO:
                print "Unbinding IB"
                self.indexbuffer.unbind()
                print "Unbinding VB"
                self.vertexbuffer.unbind()
            mat.end()

            glDepthFunc( GL_LESS )
        self.displayList = DisplayList(_commands)
        print "Done"


    def buildGrassDL(self):
        self.grassDisplayList = DisplayList( lambda: self.grassPatch.draw() )

    def drawVertexBuffer(self):
        glFrontFace( GL_CW )

        self.vertexbuffer.bind()
        self.indexbuffer.draw()
        self.vertexbuffer.unbind()

    def drawVertexArrays( self ):
        glFrontFace( GL_CW )

        #set the vertex array pointer
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointerf(  self.vertexArray )

        #set texcoord array for first texture stage
        glClientActiveTextureARB( GL_TEXTURE0_ARB )
        glEnableClientState( GL_TEXTURE_COORD_ARRAY )
        glTexCoordPointerf( self.texCoords )


        #set texcoord array for second texture stage
        glClientActiveTextureARB( GL_TEXTURE1_ARB )
        glEnableClientState( GL_TEXTURE_COORD_ARRAY )
        glTexCoordPointerf( self.worldTexCoords )

        glDrawElementsui( GL_TRIANGLES, self.indexArray )

        glDisableClientState( GL_VERTEX_ARRAY )
        glDisableClientState( GL_TEXTURE_COORD_ARRAY )
        glClientActiveTextureARB( GL_TEXTURE0_ARB )
        glDisableClientState( GL_TEXTURE_COORD_ARRAY )


    def draw( self ):
        """Draw the terrain"""
        #if self.displayList is None: self.initGraphics()

        GraphicsCard.recordDraw( ((self.size-1)*(self.size-1)*2) * len(self.material.layers) )
        #if Settings.UseShaders:

        self.displayList()

        if Settings.DrawGrass:
            #self.grassPatch.draw()
            self.grassDisplayList()
        #else:


    def drawSinglePass(self):
        """Draws the entire terrain (using basic glBegin and glEnd calls)."""
        size = float(self.size)
        glFrontFace( GL_CW )
        glDisable( GL_BLEND)
        glDisable( GL_LIGHTING )
        glBegin( GL_TRIANGLES )
        h = self.heightmap.heightData
        trr = self.textureRepeatRate
        glColor( 0.0, 0.0, 1.0, 1.0 )
        #wx = 0
        #wz = 0
        #wxn = self.spacing
        #wzn = -self.spacing
        for z in xrange( self.size-1):
            for x in xrange( self.size-1):
                wx = x * self.spacing
                wz = z * self.spacing
                wxn = (x+1) * self.spacing
                wzn = (z+1) * self.spacing
                if self.walkableMask[x+z*self.size]:
                    glColor( 1.0, 1.0, 1.0, 1.0 )
                else:
                    glColor( 0.5, 0.5, 0.5, 1.0 )
                glTexCoord2f(  float(x)/self.size, 1.0-float(z)/self.size )
                glVertex3f( wx, h[x+z*self.size], wz )
                glTexCoord2f( float(x)/self.size, 1.0-float(z+1)/self.size )
                glVertex3f( wx, h[x+(z+1)*self.size], wzn )
                glTexCoord2f( float(x+1)/self.size, 1.0-float(z)/self.size )
                glVertex3f( wxn, h[(x+1)+z*self.size], wz )

                glTexCoord2f( float(x)/self.size, 1.0-float(z+1)/self.size )
                glVertex3f( wx, h[x+(z+1)*self.size], wzn )
                glTexCoord2f( float(x+1)/self.size, 1.0-float(z+1)/self.size )
                glVertex3f( wxn, h[(x+1)+(z+1)*self.size], wzn )
                glTexCoord2f( float(x+1)/self.size, 1.0-float(z)/self.size )
                glVertex3f( wxn, h[(x+1)+z*self.size], wz )
                #wx  += self.spacing
                #wxn += self.spacing
            #wz -= self.spacing
            #wzn -= self.spacing
        glEnd()



    def drawPrim(self):
        """Draws the entire terrain (using basic glBegin and glEnd calls)."""
        size = float(self.size)
        glFrontFace( GL_CW )
        glBegin( GL_TRIANGLES )
        h = self.heightmap.heightData
        trr = self.textureRepeatRate
        glColor( 1.0, 1.0, 1.0, 1.0 )
        for z in range( self.size-1):
            for x in range( self.size-1):
                wx = x * self.spacing
                wz = z * self.spacing
                wxn = (x+1) * self.spacing
                wzn = (z+1) * self.spacing
                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr * x, trr * z  )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x)/self.size, float(z)/self.size )
                glVertex3f( wx, h[x+z*self.size], wz )
                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr * x, trr * (z+1) )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x)/self.size, float(z+1)/self.size )
                glVertex3f( wx, h[x+(z+1)*self.size], wzn )
                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr *(x+1), trr *z )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x+1)/self.size, float(z)/self.size )
                glVertex3f( wxn, h[(x+1)+z*self.size], wz )

                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr * x, trr * (z+1) )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x)/self.size, float(z+1)/self.size )
                glVertex3f( wx, h[x+(z+1)*self.size], wzn )
                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr * (x+1), trr * (z+1) )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x+1)/self.size, float(z+1)/self.size )
                glVertex3f( wxn, h[(x+1)+(z+1)*self.size], wzn )
                glMultiTexCoord2fARB( GL_TEXTURE0_ARB, trr * (x+1), trr * z )
                glMultiTexCoord2fARB( GL_TEXTURE1_ARB, float(x+1)/self.size, float(z)/self.size )
                glVertex3f( wxn, h[(x+1)+z*self.size], wz )
        glEnd()
