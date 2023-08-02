from PIL import Image
import os

pics = []
for path, subdirs, files in os.walk("icons"):
    for name in files:
        Image.open(os.path.join(path, name))

# Get the size of the image
width, height = picture.size()

# Process every pixel
for x in width:
   for y in height:
       current_color = picture.getpixel( (x,y) )
       ####################################################################
       # Do your logic here and create a new (R,G,B) tuple called new_color
       ####################################################################
       picture.putpixel( (x,y), new_color)