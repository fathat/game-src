import Graphics
from Graphics import GraphicsCard


class Light(object):
    def enable(lightnum):
        GraphicsCard.enable('light'+str(lightnum))

class DirectionalLight(object):
    pass

class PointLight(object):
    pass

