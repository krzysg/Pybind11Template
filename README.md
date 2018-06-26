# Pybind11Template
Template cmake project for playing with pybind11

# To build:
```
mkdir build
cd build
cmake ..
make
```

# To play with example you can run such a script in a build directory
``` python
import numpy as np
from pyTemplate import *

x=MyClassShort()
x.size()

# Uses same data buffer as CPP side
img=np.array(x, copy = False)
img
x.showData()
img[0] = 123;
img
x.showData()

# Copies data from CPP side
img=np.array(x, copy = True)
img
x.showData()
x.showData()
img[0] = 666;
img
x.showData()
```
