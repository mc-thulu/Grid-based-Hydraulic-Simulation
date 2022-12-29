#ifndef EXDIMUM_MANNING_H
#define EXDIMUM_MANNING_H

#include <vector>

#include "simulation_data.hpp"

namespace gbhs {

class Manning {
   public:
    Manning(SimulationData& data) : data(data) {}
    void step(const float& dt);

   private:
    SimulationData& data;
    std::vector<std::pair<size_t, float>> l_inflow;
    std::vector<std::pair<size_t, float>> l_outflow;
};

}  // namespace gbhs

#endif
