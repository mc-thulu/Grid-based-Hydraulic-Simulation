import struct
import numpy as np
from matplotlib import pyplot as plt

path = "build/Debug/metadata.bin"
f = open(path, "rb")

# see https://docs.python.org/3/library/struct.html for details
# metadata
# TODO c++ padding???
raw_data = f.read(struct.calcsize("<iiiiffQ")) # file layout
data = struct.unpack("<iiiiffQ", raw_data)

# height data (array)
width = data[2]
height = data[3]
raw_data = f.read(struct.calcsize("<f") * width * height)
f.close()
height_data = struct.unpack("f" * width * height, raw_data)

# visualize height data
# arr = np.array(height_data)
# arr2d = np.reshape(arr, (height, width)) # todo order?

# read water level data
path = "build/Debug/step_0.bin"
f = open(path, "rb")
raw_data = f.read(struct.calcsize("<Q")) # file layout
length = struct.unpack("<Q", raw_data)[0]
raw_data = f.read((struct.calcsize("<Qff")) * length)
water_data = struct.unpack("Qff" * length, raw_data)
f.close()

# visualize water level data
x = []
y = []
h = []
for i in range(0, length):
    x.append(water_data[i*3] % width)
    y.append(int(water_data[i*3]/height))
    h.append(water_data[i*3+1])

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
for m, zlow, zhigh in [('o', -50, -25), ('^', -30, -5)]:
    xs = x
    ys = y
    zs = h
    ax.scatter(xs, ys, zs, marker=m)
plt.show()
