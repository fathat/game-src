import pygame
import GlTest
import Camera
import MeshLoader
from Graphics import GraphicsCard
from cgkit.cgtypes import *
from MathUtil import degreeToRadian

class TerrainTestApp(GlTest.GlTestApp):
    move_speed = 2
    
    def onInit(self):
        #self.camera = Camera.Freecam()
        self.mesh = MeshLoader.LoadMeshFromFile('../base/art/objects/objects/crate.s3d')

    def onUpdate(self, timedelta):
        pass

    def onDraw(self):
        self.camera.apply()
        self.mesh.draw()

if __name__ == '__main__':
    GlTest.run( TerrainTestApp)
