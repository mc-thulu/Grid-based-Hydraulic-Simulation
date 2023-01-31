#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "definitions.hpp"
#include "gdal_priv.h"
#include "manning.hpp"
#include "perlin_noise.hpp"
#include "simulation_data.hpp"
#include "utils.hpp"

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using CHRONO_UNIT = std::chrono::milliseconds;
using namespace gbhs::constants;

void readGDALData(const char* file,
                  gbhs::SimulationData& simulation_data,
                  float* buffer,
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
        dataset->GetRasterBand(1);          // assume that there is only one band
    auto res = band->RasterIO(GF_Read,      // mode
                              offset_x,     // offset x
                              offset_y,     // offset y
                              width,        // size x
                              height,       // size y
                              buffer,       // buffer
                              width,        // buffer size x
                              height,       // buffer size y
                              GDT_Float32,  // format
                              0,            // pixel space
                              0u);          // line space
    if (res == CE_Failure) {
        std::cerr << "GDAL file could not be read." << std::endl;
        exit(1);
    }
    GDALClose(dataset);

    for (int iy = 0; iy < height; ++iy) {
        for (int ix = 0; ix < width; ++ix) {
            size_t idx = ix + iy * width;
            gbhs::Cell& c = simulation_data.getCell(ix, iy);
            c.height = buffer[idx];

            // find neigbor
            gbhs::Vec2i lowest_neighbour_idx = {0, 0};
            float lowest_neighbour_distance = 0;
            float lowest_gradient = 0;
            for (int ny = std::max(0, iy - 1); ny < std::min(iy + 2, height); ++ny) {
                for (int nx = std::max(0, ix - 1); nx < std::min(ix + 2, width); ++nx) {
                    if (iy == ny && ix == nx) {
                        continue;
                    }

                    size_t neighbor_idx = nx + ny * width;
                    if (buffer[neighbor_idx] < 0.0f) {
                        continue;
                    }

                    float distance = sqrtf((ix - nx) * (ix - nx) + (iy - ny) * (iy - ny));
                    float height_dif = (buffer[neighbor_idx] - buffer[idx]);
                    float gradient = height_dif / distance;

                    if (gradient < lowest_gradient) {
                        lowest_gradient = gradient;
                        lowest_neighbour_idx = {nx, ny};
                        lowest_neighbour_distance = distance;
                    }
                }
            }

            // was a neighbour found?
            if (lowest_gradient < 0.0f) {
                c.neighbor = &simulation_data.getCell(
                    lowest_neighbour_idx.x,
                    lowest_neighbour_idx.y);  // TODO pointer to ref???
                c.distance = lowest_neighbour_distance;
                c.gradient = std::abs(lowest_gradient);
            }
        }
    }
}

// ------------------------------------------------

void decideRainCells(std::vector<std::pair<uint32_t, double>>& rain_cells,
                     gbhs::SimulationData& data,
                     gbhs::Vec2ui offset) {
    // decide rain cells
    rain_cells.clear();
    const siv::PerlinNoise::seed_type seed = PERLIN_NOISE_SEED;
    const siv::PerlinNoise perlin{seed};
    for (int y = 0; y < data.dimensions.y; ++y) {
        for (int x = 0; x < data.dimensions.x; ++x) {
            float noise = perlin.noise2D_01((double)(x + offset.x) / PERLIN_NOISE_SCALE,
                                            (double)(y + offset.y) / PERLIN_NOISE_SCALE);
            if (noise > 0.7f) {
                int block_x = x / BLOCKSIZE_X;
                int block_y = y / BLOCKSIZE_Y;
                int cell_x = x - block_x * BLOCKSIZE_X;
                int cell_y = y - block_y * BLOCKSIZE_Y;
                gbhs::Block& b = data.blocks[block_y][block_x];
                gbhs::Cell& c = b.data[cell_y][cell_x];
                if (c.height < 0.f) {
                    continue;
                }
                // TODO: rain depends on cell size; for now: assume cell = 1m^2
                c.rain =
                    (noise - 0.7) * 3.333 * RAINFALL_HEIGHT;  // [m per s per m^2]
                b.cells_with_water.set(cell_x + cell_y * BLOCKSIZE_X, true);
            }
        }
    }
}

// ------------------------------------------------

void writeMetadata(const gbhs::SimulationSettings& settings,
                   float* data,
                   const size_t& size) {
    // print map
    const char* filename = "output/metadata.bin";
    std::ofstream ws(filename, std::ios::binary);
    if (!ws.is_open()) {
        std::cout << "Error opening the file '" << filename << "'!" << std::endl;
        exit(1);
    }
    ws.write(reinterpret_cast<const char*>(&settings), sizeof(gbhs::SimulationSettings));
    ws.write(reinterpret_cast<const char*>(data), sizeof(float) * size);
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
    float* buffer = new float[settings.width * settings.height];
    readGDALData(filepath,
                 data,
                 buffer,
                 settings.offset_x,
                 settings.offset_y,
                 settings.width,
                 settings.height);
    gbhs::Manning sim(data);
    std::vector<std::pair<uint32_t, float>> output_data;
    writeMetadata(settings, buffer, settings.height * settings.width);
    delete[] buffer;

    // add initial rain
    std::vector<std::pair<uint32_t, double>> rain_cells;
    decideRainCells(rain_cells, data, {0, 0});

    // run simulation
    auto t_start = high_resolution_clock::now();
    auto t_step_start = high_resolution_clock::now();
    size_t output_counter = settings.output_resolution;  // [steps]
    for (size_t i = 0; i < SIMULATION_STEPS; ++i) {
        sim.step(settings.dt);

        // debug info
        auto t_step =
            duration_cast<CHRONO_UNIT>(high_resolution_clock::now() - t_step_start);
        t_step_start = high_resolution_clock::now();
        float fps = 1000.f / t_step.count();  // TODO avoid constant
        std::cout << "step " << i << ": " << fps << "fps; " << std::endl;

        // sweep empty cells & output
        if (--output_counter == 0) {
            output_counter = settings.output_resolution;
            std::cout << "------" << std::endl;

            // TODO rework
            size_t output_size = 0;
            for (int by = 0; by < BLOCKCNT_Y; ++by) {
                for (int bx = 0; bx < BLOCKCNT_X; ++bx) {
                    gbhs::Block& b = data.blocks[by][bx];
                    if (!b.containsWater()) {
                        continue;
                    }

                    for (int iy = 0; iy < BLOCKSIZE_Y; ++iy) {
                        for (int ix = 0; ix < BLOCKSIZE_X; ++ix) {
                            gbhs::Cell& c = b.data[iy][ix];
                            if (c.water_level > 0.f) {
                                ++output_size;
                                uint32_t idx = ix + BLOCKSIZE_X * bx +
                                               (iy + BLOCKSIZE_Y * by) * settings.width;
                                output_data.push_back({idx, c.water_level});
                            }
                        }
                    }
                }
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
