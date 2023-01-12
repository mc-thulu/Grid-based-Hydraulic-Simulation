import struct
import numpy as np
from matplotlib import pyplot as plt

path = "build/output/metadata.bin"
f = open(path, "rb")

# see https://docs.python.org/3/library/struct.html for details
# metadata
# TODO c++ padding???
raw_data = f.read(struct.calcsize("<iiiiffQ"))  # file layout
data = struct.unpack("<iiiiffQ", raw_data)

# height data (array)
width = data[2]
height = data[3]
raw_data = f.read(struct.calcsize("<f") * width * height)
f.close()
height_data = struct.unpack("f" * width * height, raw_data)

# visualize height data
arr = np.repeat(np.array(height_data), 3)  # 3 channels for RGB
arr[arr < 0.] = 0.0
arr2d = np.reshape(arr, (height, width, 3))
arr2d = ((arr2d - arr2d.min()) * (1/(arr2d.max() - arr2d.min()) * 255)
         ).astype('uint8')  # scale to 0-255
imgplot = plt.imshow(arr2d)
plt.show()

filepath = "build/output/step_"
for i in range(0, 10):
    # reset
    arr2d = np.zeros((height, width))

    # read water level data
    ifilepath = filepath + str(i) + ".bin"
    f = open(ifilepath, "rb")
    raw_data = f.read(struct.calcsize("<I"))  # file layout
    length = struct.unpack("<I", raw_data)[0]
    raw_data = f.read((struct.calcsize("<II")) * length)
    water_data = struct.unpack("II" * length, raw_data)
    f.close()

    # visualize water level data
    blue = np.array([0, 0, 255])
    white = np.array([255, 255, 255])
    for i in range(0, length):
        x = water_data[i*2] % width
        y = int(water_data[i*2] / height)
        h = water_data[i*2+1]
        # opacity = h / 1000  # TODO water level scale
        # new_color = (opacity * blue + (1-opacity)
        #              * arr2d[y][x]).astype(np.int64)
        # new_color = white * opacity
        arr2d[y][x] = h

    print(arr2d.max())
    arr2d = ((arr2d - arr2d.min()) * (1/(arr2d.max() - arr2d.min()) * 255)
            ).astype('uint8')  # scale to 0-255
    imgplot = plt.imshow(arr2d)
    plt.show()
