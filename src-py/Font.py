import gl2D
import string
import TextureManager
from TextUtil import *

from OpenGL.GL import *
from DisplayList import DisplayList


class CharInfo( object ):
    """A single character in a TextureFont"""
    __slots__ = ('id',
                 'x', 'y',
                 'width', 'height',
                 'xoffset', 'yoffset',
                 'xadvance',
                 'page',
                 'displayList',
                 '__weakref__')

    def __init__( self, tokens ):
        self.displayList = None
        for t in tokens:
            attr, val = parse_equals_statement ( t )
            setattr( self, attr, val )

    def buildDisplayList(self, texturePages):
        def renderChar():
            texpage = texturePages[self.page]
            glBegin(GL_QUADS)
            drawQuad(texpage.width, texpage.height,      #texture width, texture height
                     self.xoffset, self.yoffset, #destination xy
                     self.x, self.y,             #source xy
                     self.width, self.height )   #source width/height
            glEnd()
        self.displayList = DisplayList( renderChar )

    def draw(self):
        self.displayList.execute()



class KerningPair( object ):
    __slots__ = ('first', 'second', 'amount', '__weakref__')
    def __init__(self, tokens ) :
        for t in tokens:
            attr, val = parse_equals_statement ( t )
            setattr( self, attr, val )

    def getkey( self ):
        return ( str(self.first), str(self.second) )

    key = property( getkey, None )


class TextureFont(object):
    """A font rendered in OpenGL using textures. Supply it with a .fnt file."""

    def __init__( self, filename ):
        #Parse the .fnt file
        f = open( filename, 'r' )
        line_number = 0
        self.optimizedStrings = {}
        self.chars = {}
        self.kerning = {}
        for line in f:
            self._parseLine( line, line_number )
            line_number += 1

        #read in the texture(s)
        self.texture_pages = []
        for i in range(self.pages):
            tname = filename.replace('.fnt', '') + '_' + str(i).zfill(2) + '.tga'
            self.texture_pages.append ( TextureManager.GetTexture( tname )  )

        #build display list
        [c.buildDisplayList(self.texture_pages) for c in self.chars.values() ]

    def _parseLine( self, line, line_number ):
        """Parses a line in the font description file"""
        tokens = line.split(  )
        cmd = tokens.pop(0)
        if cmd == 'info':
            self._parseInfoOrCommonLine( tokens )
        elif cmd == 'common':
            self._parseInfoOrCommonLine( tokens )
        elif cmd == 'char':
            self._parseCharLine( tokens )
        elif cmd == 'kerning':
            self._parseKerningLine( tokens )
        else:
            print "Line %s could not be parsed" % line_number

    def _parseCharLine( self, tokens ):
        c = CharInfo( tokens )
        self.chars[ c.id ] = c

    def _parseKerningLine( self, tokens ):
        k = KerningPair( tokens )
        self.kerning[ k.key ] = k.amount

    def _parseInfoOrCommonLine( self, tokens ):
        for token in tokens:
            attr, val = parse_equals_statement( token )
            setattr( self, attr, val )

    def makeDisplayListforString(self, text):
        print """ Making display list for... "%s" """ % (text)
        size = []
        def build_dl():
            w,h = self.drawPrim(0, 0, text)
            size.extend([w,h])
        display_list = DisplayList( build_dl )
        self.optimizedStrings[text] = (display_list, w, h)
        return display_list, w, h

    def pixelSizeOf(self, text):
        screen_height = gl2D.screenSize()[1]
        cx = 0; cy = 0
        last_character = 0
        current_page = 0
        texwidth = self.texture_pages[current_page].width
        texheight = self.texture_pages[current_page].height
        for c in text:
            if ord(c) == 10:
                cy += self.size
                cx = x
                continue
            try:
                ch = self.chars[ord(c)]
            except KeyError:
                ch = self.chars[0]
            kerning_key = (last_character,ch.id)
            if kerning_key in self.kerning:
                k_amt = self.kerning[ kerning_key ]
                cx += int(k_amt)
            if ch.page != current_page:
                current_page = ch.page
                texwidth = self.texture_pages[current_page].width
                texheight = self.texture_pages[current_page].height
            cx += ch.xadvance
            last_character = ord(c)
        return (cx, cy)

    def draw(self, x, y, text, static=False, max_width=None):
        """Draw the font"""
        glEnable(GL_TEXTURE_2D)
        glEnable( GL_ALPHA_TEST)
        glAlphaFunc(GL_GREATER,0.5)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
        glDisable(GL_CULL_FACE)
        #glDisable(GL_BLEND)
        if not static:
            return self.drawPrim( x, y, text, max_width )
        else:
            try:
                dl, w, h = self.optimizedStrings[text]
            except:
                dl, w, h = self.makeDisplayListforString(text)
            gl2D.start2D()
            glPushMatrix()
            glLoadIdentity()
            glTranslatef( x, y, 0 )
            dl()
            glPopMatrix()
            gl2D.end2D()
            return w, h

    def drawPrim(self, x, y, text, max_width=None ):
        screen_height = gl2D.screenSize()[1]
        gl2D.start2D()
        cx = x
        cy = y
        last_character = 0
        current_page = 0
        self.texture_pages[current_page].bind()
        texwidth = self.texture_pages[current_page].width
        texheight = self.texture_pages[current_page].height
        glPushMatrix()
        glTranslatef(cx, screen_height-cy, 0)
        #glBegin(GL_QUADS)
        for c in text:
            if ord(c) == 10:
                cy += self.size
                glTranslatef( -(cx-x), -self.size, 0)
                cx = x
                continue
            ch = self.chars[ord(c)]
            kerning_key = (last_character,ch.id)
            if kerning_key in self.kerning:
                k_amt = self.kerning[ kerning_key ]
                cx += int(k_amt)
                glTranslatef(cx, 0, 0)

            if ch.page != current_page:
                current_page = ch.page
                self.texture_pages[current_page].bind()
                texwidth = self.texture_pages[current_page].width
                texheight = self.texture_pages[current_page].height

            #gl2D.drawSubTexture2D( self.texture_pages[current_page],  cx + ch.xoffset, cy + ch.yoffset, ch.x, ch.y, ch.width, ch.height )
            #drawQuad( texwidth, texheight, cx+ch.xoffset, cy+ch.yoffset, ch.x, ch.y, ch.width, ch.height )
            #glPushMatrix()
            #glTranslatef(cx, screen_height-cy, 0)
            if max_width and (cx-x)+ch.xadvance > max_width:
                break
            ch.draw()
            #glPopMatrix()
            cx += ch.xadvance

            glTranslatef( ch.xadvance, 0, 0 )
            last_character = ord(c)
        #glEnd()
        glPopMatrix()
        gl2D.end2D()
        return (cx, cy)

def drawQuad( texwidth, texheight, dx, dy, sx, sy, sw, sh ):
    u1 = float(sx)/texwidth
    v1 = float(sy)/texheight
    u2 = float((sx+sw))/texwidth
    v2 = float((sy+sh))/texheight
    y = gl2D.screenSize()[1] - dy

    glTexCoord2f( u1, v2 )
    glVertex3f( dx, y-sh, -.1 )
    glTexCoord2f( u2, v2 )
    glVertex3f(  dx+sw, y-sh, -.1 )
    glTexCoord2f( u2, v1 )
    glVertex3f(  dx+sw, y, -.1 )
    glTexCoord2f( u1, v1 )
    glVertex3f( dx, y, -.1 )
