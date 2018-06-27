from __future__ import print_function
import numpy as np
from pyTemplate import *

print("----------------- OBJ created -----------------")
x=MyClassShort(8)
print("Size: ", len(x))

print("\n\n--------------- SHARED MEM --------------------")
# Sharing memory 
img=np.array(x, copy = False)
print(img)
print(x)
img[0] = 123;
print(img)
print(x)

print("\n\n------------------- COPY ----------------------")
# Copying object
img=np.array(x, copy = True)
print(img)
print(x)
img[0] = 666;
print(img)
print(x)

