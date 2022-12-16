#ifndef EXDIMUM_SIMULATION_DATA_H
#define EXDIMUM_SIMULATION_DATA_H

#include <vector>

#include "utils.hpp"

namespace gbhs {

struct Cell {
    size_t x = 0;
    size_t y = 0;
    float height = 0.0f;
    float water_level = 0.0f;
    std::vector<size_t> neighbours = {};
    bool active = false;

    Cell() = default;
    Cell(const size_t& x, const size_t& y) : x(x), y(y) {}
};

class SimulationData {
   public:
    SimulationData(const Array2D<float>& height_map);

    void setWaterLevel(const size_t& cell_idx, const float& amount);
    void modifyWaterLevel(const size_t& cell_idx, const float& amount);
    // float getWaterLevel() const;
    void sweepCellsWithWater();
    size_t cellCount() const { return cells.size(); }
    const Cell& getCell(const size_t& idx) const { return cells.at(idx); }
    float cellGradient(const size_t& cell_idx1, const size_t& cell_idx2) const;
    float cellDistance(const size_t& cell_idx1, const size_t& cell_idx2) const;
    const Cell& getCell(const size_t& x, const size_t& y) const {
        return cells.at(x, y);
    }
    const std::vector<size_t>& cellsWithWater() const {
        return cells_with_water;
    }

   private:
    Array2D<Cell> cells;
    std::vector<size_t> cells_with_water;  // store idx of cell in cells array
};

}  // namespace gbhs

#endif
