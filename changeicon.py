from PIL import Image
import os

patron = Image.open("512.png")
pics = []
i=0
for path, subdirs, files in os.walk("icons"):
    for name in files:
        picture = Image.open(os.path.join(path, name))

# Get the size of the image
        width, height = picture.size
        filter = patron.resize((width, height))

        # Process every pixel
        for x in range(width):
            for y in range(height):
                current_color = picture.getpixel( (x,y) )
                a = filter.getpixel((x,y))
                b = (a[0],a[1],a[2],current_color[3])
                
                    #print(current_color)

                ####################################################################
                # Do your logic here and create a new (R,G,B) tuple called new_color
                ####################################################################
                    
                picture.putpixel( (x,y),b )
        picture.save(os.path.join(path, name))
        print(i)
        i+=1