#ifndef EXDIMUM_SIMULATION_DATA_H
#define EXDIMUM_SIMULATION_DATA_H

#include <bitset>

#include "definitions.hpp"
#include "utils.hpp"

using namespace gbhs::constants;

namespace gbhs {

// TODO reduce size?
struct Cell {
    float height = -1.f;
    float water_level = 0.f;
    float water_level_change = 0.f;
    Cell* neighbor = nullptr;
    float distance = 0.f;
    float gradient = 0.f;
    float rain = 0.f;
};

struct Block {
    Array2D<Cell, BLOCKSIZE_Y, BLOCKSIZE_X> data;
    std::bitset<BLOCKSIZE> cells_with_water = 0b0;
    bool containsWater() const { return cells_with_water.any(); }
};

struct SimulationSettings {
    int32_t offset_x = DATA_OFFSET_X;
    int32_t offset_y = DATA_OFFSET_Y;
    int32_t width = WORLDSIZE_X;  // TODO rework
    int32_t height = WORLDSIZE_Y;
    float dt = DT;                                 // [sec]
    size_t output_resolution = OUTPUT_RESOLUTION;  // [steps]
};

// TODO rework & visibility
class SimulationData {
   public:
    SimulationData(const size_t& width, const size_t& height);
    Cell& getCell(const size_t& x, const size_t& y);

    // TODO visibility
    Vec2ui dimensions;
    Array2D<Block, BLOCKCNT_Y, BLOCKCNT_X> blocks;
};

}  // namespace gbhs

#endif
