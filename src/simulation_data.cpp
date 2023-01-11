#include "simulation_data.hpp"

#include <algorithm>
#include <cmath>

namespace gbhs {

SimulationData::SimulationData(const size_t& width, const size_t& height) {
    height_map = Array2D<float>(width, height);
    cells = Array2D<Cell>(width, height);
    dimensions = {width, height};  // TODO min dimension 3x3
}

void SimulationData::findNeighbours() {
    for (int iy = 0; iy < dimensions.y; ++iy) {
        for (int ix = 0; ix < dimensions.x; ++ix) {
            size_t cell_idx = ix + iy * dimensions.x;
            // ignore novalue cells
            if (height_map[cell_idx] < 0.0f) {
                continue;
            }

            // find steepest neighbour
            cells[cell_idx] = Cell(ix, iy);
            size_t lowest_neighbour_idx = 0;
            float lowest_gradient = 0;
            for (int ny = std::max(0, iy - 1);
                 ny < std::min(iy + 2, (int)height_map.height);
                 ++ny) {
                for (int nx = std::max(0, ix - 1);
                     nx < std::min(ix + 2, (int)height_map.width);
                     ++nx) {
                    if (iy == ny && ix == nx) {
                        continue;
                    }

                    size_t neighbor_idx = nx + ny * dimensions.x;
                    if (height_map[neighbor_idx] < 0.0f) {
                        continue;
                    }

                    float gradient = cellGradient(neighbor_idx, cell_idx) /
                                     sqrtf((ix - nx) * (ix - nx) + (iy - ny) * (iy - ny));

                    // if (gradient >= 0.f) {
                    //     cells[cell_idx].higher_neigbours.push_back(neighbor_idx);
                    // }

                    if (gradient < lowest_gradient) {
                        lowest_gradient = gradient;
                        lowest_neighbour_idx = neighbor_idx;
                    }
                }
            }

            // was a neighbour found?
            if (lowest_gradient < 0.0f) {
                cells[cell_idx].neighbours.push_back(lowest_neighbour_idx);
            }
            // std::sort(cells[cell_idx].higher_neigbours.begin(),
            //           cells[cell_idx].higher_neigbours.end(),
            //           [&](const size_t& idx_1, const size_t& idx_2) {
            //               return height_map[idx_1] < height_map[idx_2];
            //           });
        }
    }
}

float SimulationData::cellDistance(const size_t& cell_idx1,
                                   const size_t& cell_idx2) const {
    const Cell& c1 = cells[cell_idx1];
    const Cell& c2 = cells[cell_idx2];
    return sqrtf((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

float SimulationData::cellGradient(const size_t& cell_idx1,
                                   const size_t& cell_idx2) const {
    const Cell& c1 = cells[cell_idx1];
    const Cell& c2 = cells[cell_idx2];
    return (height_map[cell_idx1] - height_map[cell_idx2]) /
           sqrtf((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

void SimulationData::sweepCellsWithWater() {
    cells_with_water.clear();
    for (int y = 0; y < dimensions.y; ++y) {
        for (int x = 0; x < dimensions.x; ++x) {
            size_t idx = x + y * dimensions.x;
            if (height_map[idx] < 0.f) {
                continue;
            }
            if (cells[idx].water_level > 0.f) {
                cells_with_water.push_back(idx);
            }
        }
    }
}

void SimulationData::setWaterLevel(const size_t& cell_idx, const float& amount) {
    Cell& c = cells[cell_idx];
    c.water_level = amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

void SimulationData::modifyWaterLevel(const size_t& cell_idx, const float& amount) {
    Cell& c = cells[cell_idx];
    c.water_level += amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

}  // namespace gbhs
