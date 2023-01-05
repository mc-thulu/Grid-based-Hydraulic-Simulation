#include "simulation_data.hpp"

#include <algorithm>
#include <cmath>

namespace gbhs {

SimulationData::SimulationData(const size_t& width, const size_t& height) {
    height_map = Array2D<float>(width, height);
    cells = Array2D<Cell>(width, height);
}

void SimulationData::findNeighbours() {
    for (int iy = 0; iy < height_map.height; ++iy) {
        for (int ix = 0; ix < height_map.width; ++ix) {
            Cell c(ix, iy);

            // find steepest neighbour
            size_t neighbour_idx = 0;
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

                    float gradient = (height_map.at(nx, ny) - height_map.at(ix, iy)) /
                                     sqrtf((ix - nx) * (ix - nx) + (iy - ny) * (iy - ny));

                    if (gradient >= 0.f) {
                        c.higher_neigbours.push_back(height_map.idx(nx, ny));
                    }

                    if (gradient < lowest_gradient) {
                        lowest_gradient = gradient;
                        neighbour_idx = height_map.idx(nx, ny);
                    }
                }
            }

            // was a neighbour found?
            if (lowest_gradient < 0.0f) {
                c.neighbours.push_back(neighbour_idx);
            }
            std::sort(c.higher_neigbours.begin(),
                      c.higher_neigbours.end(),
                      [&](const size_t& idx_1, const size_t& idx_2) {
                          return height_map.at(idx_1) < height_map.at(idx_2);
                      });
            cells.at(ix, iy) = c;
        }
    }
}

float SimulationData::cellDistance(const size_t& cell_idx1,
                                   const size_t& cell_idx2) const {
    const Cell& c1 = cells.at(cell_idx1);
    const Cell& c2 = cells.at(cell_idx2);
    return sqrtf((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

float SimulationData::cellGradient(const size_t& cell_idx1,
                                   const size_t& cell_idx2) const {
    const Cell& c1 = cells.at(cell_idx1);
    const Cell& c2 = cells.at(cell_idx2);
    return (height_map.at(cell_idx1) - height_map.at(cell_idx2)) /
           sqrtf((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

void SimulationData::sweepCellsWithWater() {
    for (int i = -1 + cells_with_water.size(); i >= 0; --i) {
        if (cells.at(cells_with_water[i]).water_level <= 0.f) {
            cells.at(cells_with_water[i]).active = false;
            cells_with_water.erase(cells_with_water.begin() + i);
        }
    }
}

void SimulationData::setWaterLevel(const size_t& cell_idx, const float& amount) {
    Cell& c = cells.at(cell_idx);
    c.water_level = amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

void SimulationData::modifyWaterLevel(const size_t& cell_idx, const float& amount) {
    Cell& c = cells.at(cell_idx);
    c.water_level += amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

}  // namespace gbhs
