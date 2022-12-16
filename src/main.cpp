#include <iostream>
#include <random>

#include "gdal_priv.h"
#include "manning.hpp"
#include "simulation_data.hpp"
#include "utils.hpp"

void readGDALData(const char* file,
                  void* buffer,
                  const size_t& offset_x,
                  const size_t& offset_y,
                  const size_t& width,
                  const size_t& height) {
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
    size_t offset_x = 10750;
    size_t offset_y = 13000;
    size_t width = 250;
    size_t height = 250;
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

    return 0;
}
