#include "simulation_data.hpp"

namespace gbhs {

SimulationData::SimulationData(const size_t& width, const size_t& height) {
    dimensions = {width, height};  // TODO min dimension 3x3
}

Cell& SimulationData::getCell(const size_t& x, const size_t& y) {
    int block_x = x / gbhs::constants::BLOCKSIZE_X;
    int block_y = y / gbhs::constants::BLOCKSIZE_Y;
    Block& b = blocks.get(block_x, block_y);
    return b.data.get(x - block_x * gbhs::constants::BLOCKSIZE_X, y - block_y * gbhs::constants::BLOCKSIZE_Y);
}

}  // namespace gbhs
