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
arr2d = np.reshape(arr, (width, height, 3))  # TODO shape order
arr2d = ((arr2d - arr2d.min()) * (1/(arr2d.max() - arr2d.min()) * 255)
         ).astype('uint8')  # scale to 0-255

filepath = "build/output/step_"
new_arr_copy = np.copy(arr2d)
for i in range(0, 12):
    # reset
    arr2d = np.copy(new_arr_copy)

    # read water level data
    ifilepath = filepath + str(i) + ".bin"
    f = open(ifilepath, "rb")
    raw_data = f.read(struct.calcsize("<Q"))  # file layout
    length = struct.unpack("<Q", raw_data)[0]
    raw_data = f.read((struct.calcsize("<Qff")) * length)
    water_data = struct.unpack("Qff" * length, raw_data)
    f.close()

    # visualize water level data
    blue = np.array([0, 0, 255])
    for i in range(0, length):
        x = water_data[i*3] % width
        y = int(water_data[i*3] / height)
        h = water_data[i*3+1]
        opacity = h / 60  # TODO water level scale
        new_color = (opacity * blue + (1-opacity)
                     * arr2d[y][x]).astype(np.int64)
        arr2d[y][x] = new_color
    imgplot = plt.imshow(arr2d)
    plt.show()
