import elementtree.ElementTree as ET
import string
import array
import BinaryFile
import TextureManager
import Platform
import math
import ImageLoader

class TerrainLayer(object):
    """Represents a terrain layer (mostly stores textures)"""
    def __init__(self, texture_name, mask_texture_name, lightmap_name):
        self._textureName = texture_name
        self._maskTextureName = mask_texture_name
        self._lightmapName = lightmap_name
        self._readMask(  )

    def precacheGraphics(self):
        self.texture
        self.lightmap
        self.maskTexture

    @property
    def texture(self): return TextureManager.GetTexture( self._textureName )

    @property
    def maskTexture(self): return TextureManager.GetTexture( self._maskTextureName )

    @property
    def lightmap(self): return TextureManager.GetTexture( self._lightmapName )

    def use( self ):
        self.texture.bind(0)
        self.lightmap.bind(1)
        self.maskTexture.bind(2)

    def _readMask(self):
        img = ImageLoader.load( self._maskTextureName )#self.maskTexture.image#.convert("RGBA")
        imgstring = img.convert("RGBA").tostring("raw", "RGBA", 0, 1)
        buffer = array.array('B', imgstring )
        self.maskWidth, self.maskHeight = img.size
        self.mask = buffer
        #self.mask = [1] * (self.maskWidth*self.maskHeight)
        #i1 = 0
        #i2 = 0
        #for y in xrange(self.maskHeight):
        #    for x in xrange(self.maskWidth):
        #        self.mask[i1] =  buffer[i2]/255.0
        #        #self.mask[x+y*self.maskWidth] = float( img.getpixel( (x,y) )[0]) / 255.0
        #        i1 += 1
        #        i2 += 4

    def opacityAt(self, percent_x, percent_y):
        """Returns the opacity at the point on the layer. The opacity is
        a float between 0.0 and 1.0"""
        mx = int(percent_x * self.maskWidth)
        my = int(percent_y * self.maskHeight)
        if mx>=self.maskWidth or mx < 0 or my>=self.maskHeight or my < 0:
            return 0.0
        return self.mask[mx+my*self.maskWidth*4]/255.0

class Layer(object):
    pass

def loadLayers( modname, mapname):
    path = '../%s/maps/%s/' % (modname, mapname )
    filename = path + 'layers.xml'
    layers = []
    tree = ET.parse( filename )
    root = tree.getroot()
    for l in  root.getchildren( ):
        new_layer = Layer()
        for attrs in l.getchildren():
            setattr( new_layer, attrs.tag.strip( string.whitespace) , attrs.text )
        layers.append(
                    TerrainLayer( Platform.asOSPath(new_layer.TextureName),
                                  Platform.asOSPath(path + 'layer_' + new_layer.Name + '.png'),
                                  Platform.asOSPath(path+'shadows.png') ) )
    return layers
