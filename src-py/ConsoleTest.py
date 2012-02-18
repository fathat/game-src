import GlTest
import Console
import Graphics

class ConsoleTestApp(GlTest.GlTestApp):
    def onInit(self):
        self.console = Console.Console( Graphics.Rectangle(30, 30, 512, 160 ) )
        self.setRepeatRate( 200, 50)
        self.console.setAsStdOut()
        print "WHATS UP BITCHES!!"
        print "yeah, thought so!"

    def onUpdate(self, timedelta):
        self.console.update(timedelta)

    def onKeyDown( self, keyevent ):
        #print keyevent
        self.console.onKeyPressed(keyevent)

    def onDraw(self):
        #Graphics.GraphicsCard.start2D()
        self.console.draw()
        #Graphics.GraphicsCard.end2D()


if __name__ == '__main__':
    GlTest.run( ConsoleTestApp)
