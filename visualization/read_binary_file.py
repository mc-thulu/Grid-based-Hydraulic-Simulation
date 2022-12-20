import struct
import numpy as np
from matplotlib import pyplot as plt

path = "build/Debug/metadata.bin"
f = open(path, "rb")

# see https://docs.python.org/2/library/struct.html for details
# metadata
raw_data = f.read(struct.calcsize("<QQiifQ")) # file layout
data = struct.unpack("<QQiifQ", raw_data)

# height data (array)
width = data[0]
height = data[1]
raw_data = f.read(struct.calcsize("<f") * width * height)
f.close()
height_data = struct.unpack("f" * width * height, raw_data)

# visualize height data
arr = np.array(height_data)
arr2d = np.reshape(arr, (height, width)) # todo order?
plt.imshow(arr2d, cmap='gray')
plt.show()
