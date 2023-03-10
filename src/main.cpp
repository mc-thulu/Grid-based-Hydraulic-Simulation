#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "gdal_priv.h"
#include "manning.hpp"
#include "perlin_noise.hpp"
#include "simulation_data.hpp"
#include "utils.hpp"

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using CHRONO_UNIT = std::chrono::milliseconds;

constexpr size_t simulation_steps = 1500;

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

void addRain(gbhs::SimulationData& data,
             const std::vector<std::pair<uint32_t, double>>& rain_cells) {
    for (const auto& i : rain_cells) {
        data.modifyWaterLevel(i.first, 0.0005f * i.second);
    }
}

// ------------------------------------------------

void decideRainCells(std::vector<std::pair<uint32_t, double>>& rain_cells,
                     gbhs::SimulationData& data,
                     gbhs::Vec2ui offset) {
    // decide rain cells
    rain_cells.clear();
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    for (int y = 0; y < data.dimensions.y; ++y) {
        for (int x = 0; x < data.dimensions.x; ++x) {
            float noise = perlin.noise2D_01((double)(x + offset.x) / 4000,
                                            (double)(y + offset.y) / 4000);
            if (noise > 0.7f) {
                size_t idx = x + y * data.dimensions.x;
                if (data.height_map[idx] < 0.f) {
                    continue;
                }
                rain_cells.push_back({idx, (noise - 0.7) * 3.333});
            }
        }
    }
}

// ------------------------------------------------

void writeMetadata(const gbhs::SimulationSettings& settings, gbhs::SimulationData& data) {
    // print map
    const char* filename = "output/metadata.bin";
    std::ofstream ws(filename, std::ios::binary);
    if (!ws.is_open()) {
        std::cout << "Error opening the file '" << filename << "'!" << std::endl;
        exit(1);
    }
    ws.write(reinterpret_cast<const char*>(&settings), sizeof(gbhs::SimulationSettings));
    ws.write(reinterpret_cast<const char*>(data.height_map.ptr()),
             sizeof(float) * data.height_map.size());
    ws.close();
}

// ------------------------------------------------

void writeStepData(const std::string& filename,
                   const uint32_t& size,
                   const std::vector<std::pair<uint32_t, float>>& data) {
    std::ofstream ws(filename, std::ios::binary);
    if (!ws.is_open()) {
        std::cout << "Error opening the file '" << filename << "'!" << std::endl;
        std::exit(1);
    }
    ws.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
    ws.write(reinterpret_cast<const char*>(&data[0]),
             sizeof(std::pair<uint32_t, float>) * size);
    ws.close();
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
    std::vector<std::pair<uint32_t, float>> output_data;
    writeMetadata(settings, data);

    // add initial rain
    std::vector<std::pair<uint32_t, double>> rain_cells;
    decideRainCells(rain_cells, data, {0, 0});
    addRain(data, rain_cells);

    // run simulation
    auto t_start = high_resolution_clock::now();
    auto t_step_start = high_resolution_clock::now();
    size_t output_counter = settings.output_resolution;  // [steps]
    for (size_t i = 0; i < simulation_steps; ++i) {
        sim.step(settings.dt);
        addRain(data, rain_cells);

        // debug info
        auto t_step =
            duration_cast<CHRONO_UNIT>(high_resolution_clock::now() - t_step_start);
        t_step_start = high_resolution_clock::now();
        float fps = 1000.f / t_step.count();  // TODO avoid constant
        std::cout << "step " << i << ": " << fps << "fps; "
                  << data.cellsWithWater().size() << " cells with water" << std::endl;

        // sweep empty cells & output
        if (--output_counter == 0) {
            output_counter = settings.output_resolution;
            std::cout << "------" << std::endl;

            // prepare water level data for output
            data.sweepCellsWithWater();
            size_t output_size = data.cellsWithWater().size();
            output_data.reserve(output_size);
            for (const uint32_t& idx : data.cellsWithWater()) {
                output_data.push_back({idx, data.getCell(idx).water_level});
            }

            // save water levels to disk
            std::string filename = "output/step_";
            uint32_t step_count = (int)(i / settings.output_resolution);
            filename.append(std::to_string(step_count));
            filename.append(".bin");
            writeStepData(filename, output_size, output_data);
            output_data.clear();

            // change rain
            decideRainCells(rain_cells, data, {step_count * 250, step_count * 250});
        }
    }

    // runtime measurements
    auto t_end = high_resolution_clock::now();
    auto t_diff = duration_cast<CHRONO_UNIT>(t_end - t_start);
    std::cout << "Elapsed time: " << t_diff.count() << std::endl;

    return 0;
}
