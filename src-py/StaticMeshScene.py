from Graphics import GraphicsCard
from cgkit.cgtypes import vec3, mat4
import mathhat
import Settings


class StaticMeshScene(object):
    def __init__(self, ent_list):
        """Initialize the scene"""
        #first, grab all the submeshes and associated matrix
        #and construct a dictionary where meshes are the keys, and
        #positions are the values
        self.submeshes = {}
        self.ents = [x for x in ent_list if x.isStatic]
        self.sphereManager = mathhat.SphereVisChecker()
        for e in self.ents:
            bs = e.boundingSphere
            if bs:
                e.visSphere = self.sphereManager.addSphere( bs.center.x, bs.center.y, bs.center.z, bs.radius, e )
            else:
                e.visSphere = None

    def _postEnt( self, ent ):
        transform_matrix = ent.matrix
        for submesh in ent.mesh.meshes:
            self._postSubMesh( submesh, transform_matrix )

    def _postSubMesh(self, submesh, location_matrix ):
        try:
            lst = self.submeshes[submesh]
        except KeyError:
            self.submeshes[submesh] = []
            lst = self.submeshes[submesh]
        lst.append( location_matrix )

    def _drawSubMeshes(self, submesh, matrices ):
        submesh.startDraw()
        for matrix in matrices:
            GraphicsCard.pushMatrix()
            GraphicsCard.multMatrix( matrix.toList() )
            submesh.drawBuffer()
            GraphicsCard.popMatrix()
        submesh.endDraw()

    def _drawPostedEnts( self ):
        for submesh, matrices in self.submeshes.items():
            self._drawSubMeshes( submesh, matrices)

    def _clearPosted(self):
        for key in self.submeshes.keys():
            self.submeshes[key] = []

    def _postVisibleEnts(self, camera):
        self._clearPosted()
        for e in self.ents:
            #if e.visibleIn(camera):
            if e.visSphere and e.visSphere.visible():
                self._postEnt(e)

    def drawWithVertexBuffers(self, camera):
        self.sphereManager.markVisibleSpheres(camera.frustrum)
        self._postVisibleEnts(camera)
        self._drawPostedEnts()

    def drawWithoutVB(self, camera):
        self.sphereManager.markVisibleSpheres(camera.frustrum)
        for e in self.ents:
            if e.visSphere and e.visSphere.visible():
                e.draw()

    def draw(self, camera):
        GraphicsCard.setFrontFace('CW')
        if Settings.UseVBO:
            self.drawWithVertexBuffers(camera)
        else:
            self.drawWithoutVB(camera)
