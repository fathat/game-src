import math
import os.path
import string
import ConfigParser
import Platform
import TextUtil
import Behaviors
import MathUtil
import elementtree.ElementTree as ET
from Ents import Crate, Vegetation, StaticObject
from Actor import Actor, AiActor
from cgkit.cgtypes import *

def LoadEntsFromFile( filename, world, is_server ):
    #store instance variables
    ents = []

    #Load from the XML file holding the entities
    tree = ET.parse( filename )
    root = tree.getroot()

    for ent in root.getchildren( ):
        if ent.tag == 'count': continue  #Ignore the count element

        #read all the XML data associated with the ent into an object
        instance_data = EntInstanceData(ent.getchildren())

        #read the INI files description of the ent type
        type_info = GetEntTypeInfoForTypePath(Platform.asOSPath(instance_data.typepath))

        if not is_server and type_info.pythonClass.serverside:
            continue #if we're on the client side, and this is a serverside ent
                     #don't make it
        else:
            ents.append(MakeEnt(world, instance_data, type_info))
    return ents

class EntInstanceData(object):
    """Stores initial instance-specific information about an entity, such as
    its name and its location in the world. There are two ways to initialize this:
    either by passing a list of xml elements to initialize from, or by each
    individual attribute"""

    def __init__(self, xml_elements=None, type=None, typepath=None,
                 scriptname=None, x=0, y=0, z=0, h=0, yaw=0, pitch=0, roll=0):
        if xml_elements:
            for elem in xml_elements:
                self.setValueFromXML( elem )
        else:
            self.type = type
            self.typepath = typepath
            self.scriptname = scriptname
            self.x = x
            self.y = y
            self.z = z
            self.h = h
            self.yaw = yaw
            self.pitch = pitch
            self.roll = roll


    def setValueFromXML( self, element ):
        tag  = element.tag.strip( string.whitespace)
        text = str( element.text )
        if tag == 'type' or tag == 'typepath' or tag == 'scriptname':
            setattr( self, tag, text )
        elif tag == 'location':
            self.x = float(element.attrib['x'])
            self.y = float(element.attrib['y'])
            self.z = float(element.attrib['z'])
            self.h = float(element.attrib['heightoffset'])
        elif tag == 'rotation':
            self.pitch = float(element.attrib['x'])
            self.yaw   = float(element.attrib['y'])#math.fmod( math.pi-float(element.attrib['y']), 2*math.pi)
            self.roll  = float(element.attrib['z'])

    def __str__(self):
        return "EntData \n\ttype: %s, \n\ttypepath: %s, \n\tscriptname: %s\n\txyzh: %f, %f, %f, %f\n\typr: %f, %f, %f"  % (
                    self.type, self.typepath, self.scriptname, self.x, self.y, self.z, self.h, self.yaw, self.pitch, self.roll )

class EntTypeInfo( object ):
    """Wrapper around the entity config file (usually a .ini file ). Stores
    basic information about the entity type (what mesh it uses, whether it can move, etc.)"""

    typeMappings = {
        'crate' : Crate,
        'vegetation' : Vegetation,
        'tree' : StaticObject,
        'avatar' : Actor,
        'ai' : AiActor
    }


    def __init__(self, type_path):
        #read ent config
        self.typePath = type_path
        self.typeClass = ""
        self.config = ConfigParser.ConfigParser()
        self.config.read( type_path ) #ent_xml_data.typepath
        print "Parsed Config Type:", type_path

        try:
            self.meshFile = TextUtil.remove_quotes( self.config.get("graphics", "object" ) )
        except:
            self.meshFile = None

        try:
            self.speed =  float( self.config.get("properties", "speed" ) )
        except:
            self.speed = None

        try:
            csv = TextUtil.remove_quotes( self.config.get( "properties", "behaviors")   )
            behaviors = csv.split()
            self.behaviors = [eval("Behaviors."+x) for x in behaviors]
        except ConfigParser.NoOptionError:
            self.behaviors = []

        self.typeClass = TextUtil.remove_quotes( self.config.get("properties", "type" ) )
        fname = os.path.basename( type_path )
        name = os.path.splitext(fname)[0]
        self.typeName  = name

    @property
    def pythonClass( self ):
        try:
            return_type = self.typeMappings[ self.typeClass.lower() ]
        except KeyError:
            #Default to static object if there's no class to handle
            #the given ent type
            return_type = StaticObject
        return return_type

entTypeInfoCache = {}

def GetEntTypeInfoForTypePath( type_path ):
    try:
        rval = entTypeInfoCache[type_path]
    except KeyError:
        rval = EntTypeInfo(type_path)
        entTypeInfoCache[type_path] = rval
    return rval

def MakeEnt( world, init_data, type_info=None ):
    """Creates an entity from an EntInstanceData object"""
    if not type_info:
        type_info = GetEntTypeInfoForTypePath( Platform.asOSPath(init_data.typepath) )
    return type_info.pythonClass( world, init_data, type_info )

def MakeEntFromPacket( world, packet):
    """Creates an entity from a packet. Specifically the SpawnEnt
    packet in GameProtocol.py"""
    ent = MakeEnt( world, InstanceDataFromPacket(packet) )
    ent.id = packet.entID
    return ent

def InstanceDataFromPacket( packet ):
    """Grab instance data from the packet"""
    typepath = Platform.asOSPath(packet.typepath)
    typestr = GetEntTypeInfoForTypePath(typepath).typeClass
    yaw, pitch, roll = MathUtil.quaternionToEulerYawPitchRoll( quat( packet.qw, packet.qx, packet.qy, packet.qz) )
    return EntInstanceData(None,
                           typestr,
                           typepath,
                           packet.scriptname,
                           packet.x, packet.y, packet.z,
                           yaw, pitch, roll)
