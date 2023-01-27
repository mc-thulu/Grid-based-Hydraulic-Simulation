#ifndef EXDIMUM_MANNING_H
#define EXDIMUM_MANNING_H

#include <vector>
#include <thread>

#include "simulation_data.hpp"

namespace gbhs {

class Manning {
   public:
    Manning(SimulationData& data) : data(data) {}
    void step(const float& dt);

   private:
    SimulationData& data;
    void calc(Cell& c, const float dt);
    void simulateBlocks(const float& dt, int row);
    void simulateBorders(const float& dt);
    void applyChanges(const float& dt, int row);
    std::vector<std::thread> threads;
};

}  // namespace gbhs

#endif
