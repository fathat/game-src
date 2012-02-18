import Graphics
from Graphics import GraphicsCard
import gl2D
import Font
import pygame
import Settings
import sys
import FontManager

class Key(object):
    Escape = pygame.K_ESCAPE
    Home   = pygame.K_HOME
    End    = pygame.K_END
    Return = pygame.K_RETURN
    Delete = pygame.K_DELETE
    Backspace = pygame.K_BACKSPACE
    Left  = pygame.K_LEFT
    Right = pygame.K_RIGHT
    Up    = pygame.K_UP
    Down  = pygame.K_DOWN


class Console(object):
    def __init__(self, rectangle):
        self.bounds = rectangle
        self.lines = []
        self.unprocessedMsg = []
        self.history = []
        self.historyLine = None
        self.inputLine = ""
        self.cursor = 0
        self.font = None
        self.prompt = "$> "
        self.foregroundColor = (1, 1, 1, 1 )
        self.backgroundColor = (0, .25, .5, 0.5)
        self.echoToStdout = True
        self.onInput = None

    def setAsStdOut(self):
        sys.stdout = self

    def unsetAsStdOut(self):
        sys.stdout = sys.__stdout__

    def setFromHistory(self):
        if self.historyLine == None:
            self.inputLine = ""
            self.prompt = "$>"
        else:
            self.inputLine = self.history[self.historyLine]
            self.prompt = str(self.historyLine+1)+"/"+str(len(self.history))+">"


    def prevHistory(self):
        if len(self.history) == 0:
            return
        if self.historyLine == None:
            self.historyLine = len(self.history)
        self.historyLine -= 1
        if self.historyLine < 0:
            self.historyLine = None
        self.setFromHistory()

    def nextHistory(self):
        if len(self.history) == 0:
            return
        if self.historyLine == None:
            self.historyLine = 0
        else:
            self.historyLine += 1
        if self.historyLine >= len( self.history):
            self.historyLine = None
        self.setFromHistory()

    def moveCursorLeft(self):
        self.cursor = max(self.cursor-1, 0)

    def moveCursorRight(self):
        self.cursor = min(self.cursor+1, len(self.inputLine))

    def moveToEndOfLine(self):
        self.cursor = len(self.inputLine)

    def moveToStartOfLine(self):
        self.cursor = 0

    def backspace(self):
        if self.cursor == 0: return
        self.inputLine = self.inputLine[:self.cursor-1] + self.inputLine[self.cursor:]
        self.moveCursorLeft()

    def deleteCharacter(self):
        c = self.cursor
        self.inputLine = self.inputLine[:c] + self.inputLine[c+1:]

    def finalizeLine(self):
        self.write(self.prompt + self.inputLine)
        self.write("\n")
        self.history = [self.inputLine] + self.history
        if self.onInput:
            self.onInput.__call__(self.inputLine)
        else:
            self.unprocessedMsg.append(self.inputLine)
        self.inputLine = ""

    def write(self, text):
        if len(self.lines) == 0: self.lines.append("")
        if text == '\n':
            self.lines.append("")
        else:
            if '\n' in text:
                lns = text.split('\n')
                for ln in lns:
                    self.write(ln)
                    self.write('\n')
            else:
                self.lines[-1] = self.lines[-1] + text
        #self.lines.append(text.strip('\n').strip('\r'))
        #sys.__stdout__.write( "C:"+text )
        if self.echoToStdout:
            #sys.__stdout__.write(repr(text)+'\n')
            sys.__stdout__.write(text)

    def hasMessage(self):
        return len(self.unprocessedMsg) > 0

    def nextMessage(self):
        if self.hasMessage():
            rval = self.unprocessedMsg[0]
            del self.unprocessedMsg[0]
            return rval
        return None

    def onInput(self, line):
        self.unprocessedMsg.append(line)

    def enterCharacter(self, character):
        if character == '': return
        if self.cursor < len(self.inputLine):
            self.inputLine = (self.inputLine[:self.cursor] +
                              character +
                              self.inputLine[self.cursor:])
        else:
            self.inputLine += character
        self.moveCursorRight()
        #print self.cursor

    def onKeyPressed(self, key_event):
        key = key_event.key
        if key   == Key.Left: self.moveCursorLeft()
        elif key == Key.Right: self.moveCursorRight()
        elif key == Key.Up: self.nextHistory()
        elif key == Key.Down: self.prevHistory()
        elif key == Key.Return: self.finalizeLine()
        elif key == Key.Home: self.moveToStartOfLine()
        elif key == Key.End: self.moveToEndOfLine()
        elif key == Key.Delete: self.deleteCharacter()
        elif key == Key.Backspace: self.backspace()
        elif key == Key.Escape: pass
        else:
            self.enterCharacter( key_event.unicode)


    def update(self, timedelta):
        pass

    def _drawBox(self):
        #Draw box
        gl2D.start2D()
        GraphicsCard.disable('texture_2d', 'depth_test', 'alpha_test', 'cull_face')
        GraphicsCard.enable('blend')
        self.bounds.draw( *self.backgroundColor)
        gl2D.end2D()

    def draw(self):
        if not self.font: self.font = FontManager.GetFont(Settings.ConsoleFont)
        self._drawBox()
        #draw prompt
        GraphicsCard.enableTexture(True)
        GraphicsCard.setColorRGBA(*self.foregroundColor)
        prompt = self.prompt + self.inputLine[:self.cursor] +  self.inputLine[self.cursor:]
        w, h = self.font.pixelSizeOf( self.prompt + self.inputLine[:self.cursor] )
        self.font.draw( self.bounds.left + w, self.bounds.bottom-12, "_")
        self.font.draw(self.bounds.left, self.bounds.bottom-12, prompt)

        #draw text lines
        lineheight = 10
        y = lineheight
        reversed_lines = self.lines[:]
        reversed_lines.reverse()
        max_lines_to_draw = min(int(self.bounds.height / lineheight)-1,
                                len(reversed_lines))
        for line in reversed_lines[:max_lines_to_draw]:
            self.font.draw(self.bounds.left, self.bounds.bottom-12-y, line, False, self.bounds.width)
            y += lineheight
