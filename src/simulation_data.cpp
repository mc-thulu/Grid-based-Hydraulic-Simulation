#include "simulation_data.hpp"

namespace gbhs {

SimulationData::SimulationData(const Array2D<float>& height_map) {
    cells = Array2D<Cell>(height_map.width, height_map.height);

    for (int iy = 0; iy < height_map.height; ++iy) {
        for (int ix = 0; ix < height_map.width; ++ix) {
            Cell c(ix, iy);
            c.height = height_map.at(ix, iy);

            // find steepest neighbour
            size_t neighbour_idx = 0;
            float lowest_gradient = 0;
            for (int ny = std::max(0, iy - 1);
                 ny < std::min(iy + 2, (int)height_map.height);
                 ++ny) {
                for (int nx = std::max(0, ix - 1);
                     nx < std::min(ix + 2, (int)height_map.width);
                     ++nx) {
                    float gradient = (height_map.at(nx, ny) - c.height) /
                                     sqrtf(abs(ix - nx) + abs(iy - ny));
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
    return (c1.height - c2.height) /
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

void SimulationData::setWaterLevel(const size_t& cell_idx,
                                   const float& amount) {
    Cell& c = cells.at(cell_idx);
    c.water_level = amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

void SimulationData::modifyWaterLevel(const size_t& cell_idx,
                                      const float& amount) {
    Cell& c = cells.at(cell_idx);
    c.water_level += amount;
    if (!c.active) {
        c.active = true;
        cells_with_water.push_back(cell_idx);
    }
}

}  // namespace gbhs
