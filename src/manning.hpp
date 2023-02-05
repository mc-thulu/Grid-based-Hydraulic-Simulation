#ifndef EXDIMUM_MANNING_H
#define EXDIMUM_MANNING_H

#include <vector>

#include "simulation_data.hpp"
#include "thread_pool.hpp"

namespace gbhs {

class Manning {
   public:
    Manning(SimulationData& data) : data(data) {}
    void step();

   private:
    SimulationData& data;
    void calc(Cell& c, const int change_idx);
    void simulateBlocks(Block& b);
    void simulateBorders();
    void applyChanges(Block& b);
    ThreadPool tp;
};

}  // namespace gbhs

#endif
