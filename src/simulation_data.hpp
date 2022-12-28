#ifndef EXDIMUM_SIMULATION_DATA_H
#define EXDIMUM_SIMULATION_DATA_H

#include <vector>

#include "utils.hpp"

namespace gbhs {

struct SimulationSettings {
    int32_t offset_x = 10750;
    int32_t offset_y = 13000;
    int32_t width = 250;
    int32_t height = 250;
    float dt = 0.1f;                 // [sec]
    size_t output_resolution = 100;  // [steps]
};

struct Cell {
    size_t x = 0;
    size_t y = 0;
    float water_level = 0.0f;
    std::vector<size_t> neighbours = {};
    bool active = false;

    Cell() = default;
    Cell(const size_t& x, const size_t& y) : x(x), y(y) {}
};

class SimulationData {
   public:
    SimulationData(const size_t& width, const size_t& height);
    void findNeighbours();
    void setWaterLevel(const size_t& cell_idx, const float& amount);
    void modifyWaterLevel(const size_t& cell_idx, const float& amount);
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
    Array2D<float> height_map;  // TODO visibility

   private:
    Array2D<Cell> cells;
    std::vector<size_t> cells_with_water;  // store idx of cell in cells array
};

}  // namespace gbhs

#endif
