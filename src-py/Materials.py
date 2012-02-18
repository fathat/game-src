
class CollisionClosure(object):
    def __init__(self):
        self.crate = None
        self.actor = None

def StartBoxHitPlayer( material, body1, body2, userdata ):
    userdata.crate = body1.owner
    userdata.actor = body2.owner

def InvStartBoxHitPlayer( material, body1, body2, userdata ):
    return StartBoxHitPlayer( material, body2, body1, userdata )

def ProcessBoxHit( material, contact, userdata):
    speed = material.GetContactNormalSpeed( contact )
    if abs(speed) > 100:
        print "Actor:", userdata.actor
        print "Crate:", userdata.crate
        userdata.actor.damage( abs(speed) / 2.0 )

def DefaultMaterialMap(solver):
    materialMap = {}
    materialMap['ground'] = solver.defaultGroupID()
    materialMap['actor']  = solver.makeMaterialGroupId()
    materialMap['crate']  = solver.makeMaterialGroupId()
    collisionClosure = CollisionClosure()
    solver.world.MaterialSetCollisionCallback(materialMap['crate'],
                                              materialMap['actor'],
                                              collisionClosure,
                                              StartBoxHitPlayer,
                                              ProcessBoxHit,
                                              None )
    solver.world.MaterialSetCollisionCallback(materialMap['actor'],
                                              materialMap['crate'],
                                              collisionClosure,
                                              InvStartBoxHitPlayer,
                                              ProcessBoxHit,
                                              None )

    groundID = materialMap['ground']
    crateID  = materialMap['crate']
    actorID  = materialMap['actor']

    #physics properties for actor
    solver.setDefaultFriction(groundID, actorID,   0.3, 0.002 )
    solver.setDefaultElasticity(groundID, actorID, 0.0001 )
    solver.setDefaultSoftness(groundID, actorID,   0.9 )

    #physics properties for crates
    solver.setDefaultFriction(groundID, crateID, 1.0, 0.9 )
    solver.setDefaultElasticity(groundID, crateID, 0.2 )
    solver.setDefaultSoftness(groundID, crateID, 0.1 )
    return materialMap
