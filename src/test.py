from __future__ import print_function
import numpy as np

from pyTemplate import *

print("============== SHARING CPP WITH NUMPY =========")
print("CPP created")
x=MyVectorClassShort(8)
print("Size: ", len(x))

print("\n--------------- SHARED MEM --------------------")
# Sharing memory 
img=np.array(x, copy = False)
print(img)
print(x)

print("Changing first element of np array to 123")
img[0] = 123;
print(img)
print(x)

print("Changing first element of cpp array to 666")
x[0] = 666;
print(img)
print(x)


print("\n------------------- COPY ----------------------")
# Copying object
img=np.array(x, copy = True)
print(img)
print(x)
print("Changing first element of np array to 10000")
img[0] = 10000;
print(img)
print(x)


print("\n\n\n============== SHARING NUMPY WITH CPP =========")
print("numpy OBJ created")
n=np.arange(15, dtype=np.uint16)
print("Size: ", len(n))

print("\n--------------- SHARED MEM --------------------")
# Sharing memory
y=MyVectorClassShort(n)
print(n)
print(y)

print("Changing first element of np array to 123")
n[0] = 123;
print(n)
print(y)

print("Changing first element of cpp array to 666")
y[0] = 666;
print(n)
print(y)

print("Removing numpy")
del n
print(y)
y[10]=12345
print(y)

print("\n\n-----------------  DONE -----------------------")
