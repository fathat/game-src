from OpenGL.GL import glGenLists, glNewList, glDeleteLists, glEndList, glCallList, GL_COMPILE

class IllegalDisplayListCommandException(Exception):
    """Raised whenever a call that should not go into a display list is made,
    (for example, a call that loads a texture). """

class DisplayList(object):
    """An OpenGL display list"""
    isBuildingList = False #True while executing the builder function

    def __init__(self, build_function, allow_all=False):
        self.builder = build_function
        self.id = self._genID()
        self._build()

    def __del__(self):
        try:
            glDeleteLists( self.id, 1 )
        except TypeError:
            pass

    def __call__(self, *args, **kwargs):
        glCallList( self.id )

    def _genID(self):
        return glGenLists(1)

    def _build(self):
        DisplayList.checkForInvalidCall("Nesting a display list construction inside a display lsit")
        DisplayList.isBuildingList = True
        glNewList(self.id, GL_COMPILE)
        try:
            self.builder()
        finally:
            glEndList()
            DisplayList.isBuildingList = False

    @classmethod
    def checkForInvalidCall(cls, error_text):
        if cls.isBuildingList:
            raise IllegalDisplayListCommandException(error_text)

    def execute(self):
        glCallList(self.id)



