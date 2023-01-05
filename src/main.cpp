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

std::default_random_engine generator;
void addRain(gbhs::SimulationData& data, const float& dt) {
    int ncells = 0.4 * data.height_map.width * data.height_map.height;
    std::uniform_int_distribution<int> dist_idx(0, data.height_map.size() - 1);
    for (int i = 0; i < ncells; ++i) {
        int idx = dist_idx(generator);
        data.modifyWaterLevel(idx, 10.0f * dt);
    }
}

// ------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "A mandatory file path to the geo dataset is missing." << std::endl;
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
    data.findNeighbours();
    gbhs::Manning sim(data);
    std::vector<std::pair<size_t, float>> output_data;

    // run simulation
    size_t output_counter = settings.output_resolution;  // [steps]
    addRain(data, settings.dt);
    for (size_t i = 0; i < 1200; ++i) {
        // compute in- and outflow
        // compute groundwater storage
        sim.step(settings.dt);

        if (i < 300) addRain(data, settings.dt);

        // sweep empty cells & output
        if (--output_counter == 0) {
            output_counter = settings.output_resolution;

            // prepare water level data for output
            data.sweepCellsWithWater();
            size_t output_size = data.cellsWithWater().size();
            for (const size_t& idx : data.cellsWithWater()) {
                output_data.push_back({idx, data.getCell(idx).water_level});
            }

            // save water levels to disk
            std::string filename = "output/step_";
            filename.append(std::to_string((int)(i / settings.output_resolution)));
            filename.append(".bin");
            std::ofstream ws(filename, std::ios::binary);
            if (!ws.is_open()) {
                std::cout << "Error opening the file '" << filename << "'!" << std::endl;
                return 1;
            }
            ws.write(reinterpret_cast<const char*>(&output_size), sizeof(size_t));
            ws.write(reinterpret_cast<const char*>(&output_data[0]),
                     sizeof(std::pair<size_t, float>) * output_size);
            ws.close();
            output_data.clear();
            // addRain(data, settings.dt);
        }
    }

    // print map
    const char* filename = "output/metadata.bin";
    std::ofstream ws(filename, std::ios::binary);
    if (!ws.is_open()) {
        std::cout << "Error opening the file '" << filename << "'!" << std::endl;
        return 1;
    }
    ws.write(reinterpret_cast<const char*>(&settings), sizeof(gbhs::SimulationSettings));
    ws.write(reinterpret_cast<const char*>(data.height_map.ptr()),
             sizeof(float) * data.height_map.size());
    ws.close();

    return 0;
}
