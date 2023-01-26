#ifndef EXDIMUM_SIMULATION_DATA_H
#define EXDIMUM_SIMULATION_DATA_H

#include <bitset>

#include "utils.hpp"

namespace gbhs {

// TODO rework
constexpr int32_t WORLDSIZE_X = 23558;  // TODO dynamic
constexpr int32_t WORLDSIZE_Y = 20000;  // TODO dynamic
constexpr int BLOCKSIZE_X = 200;
constexpr int BLOCKSIZE_Y = 200;
constexpr int BLOCKSIZE = BLOCKSIZE_X * BLOCKSIZE_Y;
constexpr int BLOCKCNT_X = (WORLDSIZE_X / BLOCKSIZE_X) + 1;
constexpr int BLOCKCNT_Y = (WORLDSIZE_Y / BLOCKSIZE_Y) + 1;
constexpr int BLOCKCNT = BLOCKCNT_X * BLOCKCNT_Y;

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
    int32_t offset_x = 0;
    int32_t offset_y = 0;
    int32_t width = WORLDSIZE_X;  // TODO rework
    int32_t height = WORLDSIZE_Y;
    float dt = 0.1f;                 // [sec]
    size_t output_resolution = 150;  // [steps]
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
