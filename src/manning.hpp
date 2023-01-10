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
    void fillDepressions();
};

}  // namespace gbhs

#endif
