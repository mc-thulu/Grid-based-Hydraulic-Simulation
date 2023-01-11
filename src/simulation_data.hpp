#ifndef EXDIMUM_SIMULATION_DATA_H
#define EXDIMUM_SIMULATION_DATA_H

#include <vector>

#include "utils.hpp"

namespace gbhs {

struct SimulationSettings {
    int32_t offset_x = 0;
    int32_t offset_y = 0;
    int32_t width = 23558;
    int32_t height = 20000;
    float dt = 0.1f;                 // [sec]
    size_t output_resolution = 150;  // [steps]
};

// TODO only create these information when cell has water in it
struct Cell {
    size_t x = 0;
    size_t y = 0;
    float water_level = 0.0f;
    float water_level_change = 0.0f;
    std::vector<size_t> neighbours = {};
    // std::vector<size_t> higher_neigbours = {};  // sorted
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
    const Cell& getCell(const size_t& idx) const { return cells[idx]; }
    Cell& getCell(const size_t& idx) { return cells[idx]; }  // TODO const
    float cellGradient(const size_t& cell_idx1, const size_t& cell_idx2) const;
    float cellDistance(const size_t& cell_idx1, const size_t& cell_idx2) const;
    std::vector<size_t>& cellsWithWater() { return cells_with_water; }
    Array2D<float> height_map;  // TODO visibility

   private:
    Vec2ui dimensions;
    Array2D<Cell> cells;
    std::vector<size_t> cells_with_water;  // store idx of cell in cells array
};

}  // namespace gbhs

#endif
