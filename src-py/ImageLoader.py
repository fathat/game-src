
#Import image. Note that we need this ugly bit of magic for py2exe to
#work properly with PIL, which is why we're including all the plugins and
#forcing it to be initialized.
import Image
import BmpImagePlugin
import JpegImagePlugin
import TgaImagePlugin
import PngImagePlugin
Image._initialized=2

def load(imgname):
    return Image.open(imgname)
