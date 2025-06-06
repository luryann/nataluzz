#!/usr/bin/python3
import sys
import numpy as np
from misc import *
import cv2

if len(sys.argv) < 3:
    print("Usage: iconconv <bitmapfile> <output_name>")
    raise TypeError("Usage: iconconv <bitmapfile> <output_name>")

bmp = sys.argv[1]
output = sys.argv[2]
outputFile = sys.argv[2]

modTime = os.path.getmtime(bmp)
modDate = time.ctime(modTime)

image = cv2.imread(bmp, cv2.IMREAD_UNCHANGED)

tmp = output.split("/")
array = tmp[-1].split(".")

newPicture = array[0]
print(newPicture)

width = image.shape[1]
height = image.shape[0]
channel = image.shape[2]

test = modDate.split(" ")

newMonth = test[1]
newDay   = test[2]
newYear  = test[4]

FinalModDate = ["", "", ""]

FinalModDate[0] = newMonth
FinalModDate[1] = newDay
FinalModDate[2] = newYear

FinalFinalModDate = "/".join(FinalModDate)
split = output.split(".")


#print("picture:", bmp,end="")
#print(", output:", output,end="")
#print(", outputFile:", outputFile)

print(tmp)

if output[-1] != "h":
    raise TypeError("OUTPUT FILE IS NOT A .h FILE!")

f = open(output, "wt")
f.write("/*****************************************  \n")
f.write("                   nataluzz                 \n")
f.write("                                          \n\n")
f.write("  This is a converted icon to embed into    \n")
f.write("            the kernel image.               \n")
f.write(" * Converted File: ")
f.write(bmp)
f.write("                                            \n")
f.write(" * Converted Date: ")
f.write(str(newToday))
f.write("                                            \n")
f.write(" * Icon Last Mod: ")
f.write(str(FinalFinalModDate))
f.write("                                            \n")
f.write("*****************************************/\n\n")

f.close()

x = 0
y = 0

f = open(output, "at")

f.write("uint32_t g_")
f.write(newPicture)
f.write("_data[] = {\n\t")
f.write(f"{width}, {height}, ")

for y in range(height):
    for x in range(width):

        pixel = image[y, x]
        rgba = 0

        rgba |= (pixel[0] << 0)    #blue
        rgba |= (pixel[1] << 8)    #green
        rgba |= (pixel[2] << 16)   #red

        hexWithout0x = str(hex(rgba)).split("x")

        toAdd = ["0x00", "", ", "]
        toAdd[1] = hexWithout0x[1] 

        newHex = "".join(toAdd)

        #if(pixel[2] == 0):
        #    newHex = "0xffffffff, "

        if(newHex == "0x000, "):
            newHex = "0x00000000, "
        f.write(newHex)
    f.write("\n")

f.write("};\n\n")

f.close()