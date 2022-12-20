#include <fstream>
#include <iostream>
#include <random>

#include "gdal_priv.h"
#include "manning.hpp"
#include "simulation_data.hpp"
#include "utils.hpp"

void readGDALData(const char* file,
                  void* buffer,
                  const int32_t& offset_x,
                  const int32_t& offset_y,
                  const int32_t& width,
                  const int32_t& height) {
    GDALAllRegister();
    GDALDataset* dataset = (GDALDataset*)GDALOpen(file, GA_ReadOnly);
    if (dataset == NULL) {
        // no compatible driver found
    }

    GDALRasterBand* band =
        dataset->GetRasterBand(1);  // assume that there is only one band
    band->RasterIO(GF_Read,         // mode
                   offset_x,        // offset x
                   offset_y,        // offset y
                   width,           // size x
                   height,          // size y
                   buffer,          // buffer
                   width,           // buffer size x
                   height,          // buffer size y
                   GDT_Float32,     // format
                   0,               // pixel space
                   0u);             // line space
    GDALClose(dataset);
}

// ------------------------------------------------

void addRain(gbhs::SimulationData& data) {}

// ------------------------------------------------

int main() {
    int32_t offset_x = 10750;
    int32_t offset_y = 13000;
    int32_t width = 250;
    int32_t height = 250;

    // read height data from dataset
    gbhs::Array2D<float> height_map(width, height);
    readGDALData("data/dgm1_goslar.tif",
                 height_map.data.get(),
                 offset_x,
                 offset_y,
                 width,
                 height);
    gbhs::SimulationData data(height_map);
    gbhs::Manning sim(data);

    float dt = 0.1f;                            // [sec]
    size_t output_resolution = 100;             // [steps]
    size_t output_counter = output_resolution;  // [steps]
    for (size_t i = 0; i < 12000; ++i) {
        // TODO add rain

        // compute in- and outflow
        // compute groundwater storage
        sim.step(dt);

        // sweep empty cells & output
        if (--output_counter == 0) {
            output_counter = output_resolution;
            // TODO sweep & ouput
            data.sweepCellsWithWater();
            std::cout << "TODO: Output" << std::endl;
        }
    }

    // TODO only for testing ...
    // print map
    std::ofstream wf("metadata.bin", std::ios::binary);
    if (!wf.is_open()) {
        // file could not be opened
        // TODO
        std::cout << "file open error" << std::endl;
    }
    wf.write(reinterpret_cast<const char*>(&height_map.width), sizeof(size_t));
    wf.write(reinterpret_cast<const char*>(&height_map.height), sizeof(size_t));
    wf.write(reinterpret_cast<const char*>(&offset_x), sizeof(int32_t));
    wf.write(reinterpret_cast<const char*>(&offset_y), sizeof(int32_t));
    wf.write(reinterpret_cast<const char*>(&dt), sizeof(float));
    wf.write(reinterpret_cast<const char*>(&output_resolution), sizeof(size_t));
    wf.write(reinterpret_cast<const char*>(height_map.data.get()),
             sizeof(float) * height_map.size());
    wf.close();

    return 0;
}
