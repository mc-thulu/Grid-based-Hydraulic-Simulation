#ifndef EXDIMUM_MANNING_H
#define EXDIMUM_MANNING_H

#include <vector>

#include "simulation_data.hpp"
#include "thread_pool.hpp"

namespace gbhs {

class Manning {
   public:
    Manning(SimulationData& data) : data(data) {}
    void step(const float& dt);

   private:
    SimulationData& data;
    void calc(Cell& c, const float dt);
    void simulateBlocks(float dt, int row);
    void simulateBorders(float dt);
    void applyChanges(float dt, int row);
    ThreadPool tp;
};

}  // namespace gbhs

#endif
