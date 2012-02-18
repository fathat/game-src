import Platform
import Terrain
import Heightmap
import Materials
import TerrainMaterial
import SceneManager
import TextureManager
import elementtree.ElementTree as ET
import Mod
import TextUtil
import Camera
import Physics
import Atmosphere
import Ocean
import Settings
import array
import Graphics
import threading
from GameProtocol import GameProtocol, ByteToRadian, RadianToByte
from cgkit.cgtypes import *
from MathUtil import *
import ImageLoader
from Graphics import GraphicsCard

class ProjectDesc(object):
    """Parses the project.xml file"""

    def __init__(self, modname, mapname ):
        """Initialize the project description"""
        filename = Mod.MapPath( modname, mapname ) + "project.xml"
        tree = ET.parse( filename )
        root = tree.getroot()
        self._parseLevel( root )

    def _parseLevel(self,  element ):
        """A recursive function which essentially flattens an xml file and sets
        the values to attributes of this object. Or, by example, an xml file like:
        <Root>
            <Spacing>30</Spacing>
            <Sea>
                <SeaLevel>210</SeaLevel>
                <WaterTexture>..\base\art\land\water.bmp</WaterTexture>
            </Sea>
        </Root>
        Ends up being parsed like this:
        self.Spacing  = 30
        self.SeaLevel = 210
        self.WaterTexture = '.........'

        Hackish huh? """

        for el in element.getchildren( ):
            if len( el.getchildren() ):
                self._parseLevel( el )
            elif len( el.attrib ):
                setattr( self, el.tag, el.attrib )
            else:
                setattr( self, el.tag, TextUtil.encode_string_to_value( str(el.text) ) )

class DeathBlotch(object):
    def __init__(self, location):
        self.location = location
        self.timeleft = 2.0

    def update(self, timestep):
        self.timeleft -= timestep

    def draw(self):
        GraphicsCard.enable('blend')
        GraphicsCard.setBlendFunction('src_alpha', 'one_minus_src_alpha')
        GraphicsCard.disable('lighting', 'texture_2d')
        alpha = max( float(self.timeleft / 2.0), 0 )
        GraphicsCard.setColorRGBA( alpha/2.0, 0, 0, alpha )
        Graphics.drawSphere( self.location, 4, 8, 4 )
        GraphicsCard.enable('texture_2d')

    def alive(self):
        return self.timeleft > 0

class World(object):
    """Holds everything in the current game state."""

    def __init__ ( self, modname, mapname, is_server=False, graphics_enabled=True ):
        """Initialize the world from the given map"""
        self.modname = modname
        self.mapname = mapname
        self.playerEnt = None
        self.isServer = is_server
        self.graphicsEnabled = graphics_enabled
        self.idCounter = 0

        self.ninjasKilled   = 0
        self.piratesKilled  = 0
        self.treasuresTaken = 0
        self.deathBlotches = []

        path = Mod.MapPath( modname, mapname)
        hmap = path+"heightmap.bin"
        projectdesc = ProjectDesc ( modname, mapname )
        self.projectDesc = projectdesc
        self.seaLevel = float(projectdesc.SeaLevel)
        self.solver = Physics.Solver()
        self.solver.setMinimumFramerate(10)

        #create materials
        self.materialMap = Materials.DefaultMaterialMap(self.solver)

        #setup atmosphere
        self.atmosphere = Atmosphere.Atmosphere(projectdesc)
        self._extractShadowMap()
        self.terrainMaterial = TerrainMaterial.TerrainMaterial( modname, mapname )
        self.terrain = Terrain.TerrainPatch(  None, self, Heightmap.fromFile(hmap), self.terrainMaterial, projectdesc.Spacing )
        #if self.graphicsEnabled:
        #    self.terrain.initGraphics()
        self.ocean = Ocean.Ocean(float(projectdesc.SeaLevel),
                                 projectdesc.WaterTexture,
                                 self.terrain.getextent() )

        mx = self.terrain.getextent() / 2
        mz = self.terrain.getextent() / 2
        my = self.terrain.getWorldHeightValue( mx, mz ) + 2.0

        #Set the world size (Newton will disable anything that's not
        #within the world box
        extent = float(self.terrain.getextent())
        self.solver.setWorldSize( (-extent, -2000.0, -extent ), (extent, 2000.0, extent) )

        #Create scene. This should load all the entities as well
        self.scene   = SceneManager.SceneManager(self, modname, mapname)
        #set camera
        if not self.graphicsEnabled:
            self.playerEnt = None
            self.camera = None
        else:
            if self.playerEnt == None or Settings.RunPhysics == False:
                self.camera = Camera.Freecam()
                self.camera.position = vec3( mx, my, mz)
                self.camera.yaw = 0
                self.camera.pitch = 0
            else:
                self.camera = Camera.AttachableCamera( self.playerEnt )
            self.frustrum_camera = self.camera.asFreecam()
        print "World loaded"

    def generateEntID(self):
        rval = self.idCounter
        self.idCounter += 1
        return rval


    def initGraphics(self):
        self.terrain.initGraphics()
        self.scene.precacheGraphics()


    def materialForType( self, ent_type ):
        return self.materialMap[ent_type]

    def makeDeathBlotch( self, location):
        self.deathBlotches.append( DeathBlotch( location ) )

    def _extractShadowMap(self):
        #shadowMapTex = TextureManager.GetTexture( self.fsPath()+'shadows.png' )
        shadowimage = ImageLoader.load(self.fsPath()+'shadows.png')
        img = shadowimage.convert("RGBA")
        #img = shadowMapTex.image.convert("RGBA")
        imgstring = img.tostring("raw", "RGBA", 0, 1)
        buffer = array.array('B', imgstring)
        self.shadowMapWidth, self.shadowMapHeight = img.size
        self.shadowMap = [1] * (self.shadowMapWidth * self.shadowMapHeight)
        i1 = 0
        i2 = 0
        for y in xrange(self.shadowMapHeight):
            for x in xrange(self.shadowMapWidth):
                self.shadowMap[i1] = buffer[i2] / 255.0
                i1 += 1
                i2 += 4

    def lightIntensityAtPercentage( self, px, py ):
        ix = int( math.fmod(px*self.shadowMapWidth, self.shadowMapWidth) )
        iy = int( math.fmod(py*self.shadowMapWidth, self.shadowMapWidth))
        return self.shadowMap[ix+iy*self.shadowMapWidth]

    def lightIntensityAt(self, x, y):
        px = float(x/self.terrain.extent)
        py = float(y/self.terrain.extent)
        return self.lightIntensityAtPercentage( px, 1.0-py )

    def createMaterialGroups(self):
        pass

    def isMainPlayer(self, ent):
        return (self.playerEnt is ent)

    def fsPath( self ):
        return Mod.MapPath( self.modname, self.mapname)

    def getEntByName( self, name):
        return self.scene.getEntByName(name)

    def path( self, start, end):
        return self.terrain.path( start, end )

    def layerVisibility( self, layer_index, x, y ):
        """Returns the visibility of a layer at a given map point"""
        layers = self.terrainMaterial.layers
        visibility = 1.0
        for i in xrange(layer_index, len(layers)):
            if layer_index==i:
                visibility = layers[i].opacityAt(x,y)
            else:
                visibility *= (1.0-layers[i].opacityAt( x, y ))
        return visibility

    def netSpawnEnt(self, msg):
        self.scene.netSpawnEnt(msg)

    def netMoveActor(self, msg):
        self.scene.netMoveActor(msg)

    def netMoveEnt(self, msg):
        self.scene.netMoveEnt(msg)



    def makeMovePackets(self):
        def makemovepacket(ent):
            #ent.velocity actually has to make a foreign function
            #call to C to get the velocity, so we only want to call it once
            ev = ent.velocity
            return str(GameProtocol.MoveEnt(ent.entID,
                                        ent.location.x,
                                        ent.location.y,
                                        ent.location.z,
                                        ent.q4.x,
                                        ent.q4.y,
                                        ent.q4.z,
                                        ent.q4.w,
                                        ev.x, ev.y, ev.z
                                        ))
        return [e.makeUpdatePacket() for e in  self.scene.dynamicEnts]

    def update( self, time_delta ):
        if self.camera:
            self.camera.update( time_delta) #make sure camera updates first
        for blotch in self.deathBlotches:
            blotch.update(time_delta)
        self.deathBlotches = [x for x in self.deathBlotches if x.alive() ]
        #my = self.terrain.getWorldHeightValue( self.camera.position.x,
        #                                      -self.camera.position.z )
        #if self.camera.position.y < my + 2.0:
        #    self.camera.position.y = my+2.0
        self.scene.update( time_delta )
        self.ocean.update( time_delta)
        if Settings.RunPhysics:
            self.solver.update( time_delta )


    def isCameraBelowWater(self):
        return self.camera.position.y < self.seaLevel

    def pointOverWater(self, point):
        return (self.terrain.getWorldHeightValue(point.x, point.z) < seaLevel)

    def setFog(self):
        if self.isCameraBelowWater():
            self.atmosphere.setUnderwaterState()
        else:
            self.atmosphere.setState()

    def drawOcean(self):
        self.ocean.draw()

    def draw(self):
        if not self.isCameraBelowWater():
            self.atmosphere.draw(self.camera)
        self.setFog()
        GraphicsCard.disable( 'alpha_test')
        GraphicsCard.enable( 'blend' )
        GraphicsCard.setFrontFace( 'cw' )
        if Settings.DrawTerrain: self.terrain.draw()
        TextureManager.DisableStage( 1 )
        TextureManager.DisableStage( 2 )
        GraphicsCard.setFrontFace( 'ccw' )
        GraphicsCard.setAlphaFunc('greater', 0.5)
        GraphicsCard.disable( 'blend')
        GraphicsCard.enable( 'alpha_test', 'lighting')
        if Settings.DrawStaticMeshes: self.scene.draw()
        for blotch in self.deathBlotches:
            blotch.draw()

        self.drawOcean()
        self.scene.drawAiInfo()
        GraphicsCard.disable('lighting', 'fog')

if __name__ == '__main__':
    ProjectDesc( 'base', 'testmap' )
