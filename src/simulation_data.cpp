#include "simulation_data.hpp"

namespace gbhs {

SimulationData::SimulationData(const size_t& width, const size_t& height) {
    dimensions = {width, height};  // TODO min dimension 3x3
}

Cell& SimulationData::getCell(const size_t& x, const size_t& y) {
    int block_x = x / gbhs::BLOCKSIZE_X;
    int block_y = y / gbhs::BLOCKSIZE_Y;
    Block& b = blocks[block_y][block_x];
    return b.data[y - block_y * gbhs::BLOCKSIZE_Y][x - block_x * gbhs::BLOCKSIZE_X];
}

}  // namespace gbhs
