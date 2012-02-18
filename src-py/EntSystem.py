from LangExt import *

class Ent:
    def __init__(self, world, parent, ent_instance_info, ent_type_info):
        """Initializes location data and mesh filename """
        self.instanceInfo = ent_instance_info
        self.typeInfo = ent_type_info
        self.world  = world
        self.parent = parent
        self.entID = world.generateEntID()
        self.model = _createModel()
        self.controller = _createController()

    def _createModel(self):
        return IEntModel(self, self.instanceInfo, self.typeInfo)

    def _createController(self):
        return IEntController(self, self.instanceInfo, self.typeInfo)

    def draw(self):
        self.model.draw()

    def update(self):
        self.controller.update()
        self.model.update()


class IEntModel:
    def __init__(self, ent, instanceInfo, typeInfo):
        pass

    def draw(self): pass

    def update(self, timestep): pass

class IEntController:
    def __init__(self, ent, instanceInfo, typeInfo):
        pass

    def update(self, time):
        pass


#### MODELS ####################################################################

class ActorModel(IEntModel):
    pass

class CrateModel(IEntModel):
    pass

class StaticMeshModel(IEntModel):
    pass

class NullModel(IEntModel):
    pass


#### CONTROLLERS ###############################################################

class AiController(IEntController):
    pass
