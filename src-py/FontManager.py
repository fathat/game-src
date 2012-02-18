import Font

fonts = {}

def GetFont( filename ):
    fnt = fonts.get( filename, None)
    if not fnt:
        fnt = Font.TextureFont(filename)
        fonts[filename] = fnt
    return fnt
