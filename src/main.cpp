#include <fstream>
#include <iostream>
#include <random>
#include <string>

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "A mandatory file path to the geo dataset is missing."
                  << std::endl;
        return 1;
    }

    // prepare simulation
    const char* filepath = argv[1];
    gbhs::SimulationSettings settings;
    gbhs::SimulationData data(settings.width, settings.height);
    readGDALData(filepath,
                 data.height_map.ptr(),
                 settings.offset_x,
                 settings.offset_y,
                 settings.width,
                 settings.height);
    gbhs::Manning sim(data);
    std::vector<std::pair<size_t, float>> output_data;

    // run simulation
    size_t output_counter = settings.output_resolution;  // [steps]
    for (size_t i = 0; i < 1200; ++i) {
        // TODO add rain

        // compute in- and outflow
        // compute groundwater storage
        sim.step(settings.dt);

        // sweep empty cells & output
        if (--output_counter == 0) {
            output_counter = settings.output_resolution;

            // test pattern
            for (size_t iy = 0; iy < settings.height; ++iy) {
                for (size_t ix = 0; ix < settings.width; ++ix) {
                    if ((ix + iy) % 2 == 0) {
                        data.setWaterLevel(data.height_map.idx(ix, iy), 10.f);
                    }
                }
            }

            // prepare water level data for output
            data.sweepCellsWithWater();
            size_t output_size = data.cellsWithWater().size();
            for (const size_t& idx : data.cellsWithWater()) {
                output_data.push_back({idx, data.getCell(idx).water_level});
            }

            // save water levels to disk
            std::string filename = "output_";  // TODO folder
            filename.append(
                std::to_string((int)(i / settings.output_resolution)));
            filename.append(".bin");
            std::ofstream ws(filename, std::ios::binary);
            if (!ws.is_open()) {
                // file could not be opened
                std::cout << "error open file" << std::endl;
            }
            ws.write(reinterpret_cast<const char*>(&output_size),
                     sizeof(size_t));
            ws.write(reinterpret_cast<const char*>(&output_data[0]),
                     sizeof(std::pair<size_t, float>) * output_size);
            ws.close();
            output_data.clear();
        }
    }

    // print map
    std::ofstream ws("metadata.bin", std::ios::binary);
    if (!ws.is_open()) {
        // file could not be opened
        std::cout << "error open file" << std::endl;
    }
    ws.write(reinterpret_cast<const char*>(&settings),
             sizeof(gbhs::SimulationSettings));
    ws.write(reinterpret_cast<const char*>(data.height_map.ptr()),
             sizeof(float) * data.height_map.size());
    ws.close();

    return 0;
}
