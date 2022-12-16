#ifndef EXDIMUM_MANNING_H
#define EXDIMUM_MANNING_H

#include "simulation_data.hpp"

namespace gbhs {

class Manning {
   public:
    Manning(SimulationData& data) : data(data) {}
    void step(const float& dt);

   private:
    SimulationData& data;
};

}  // namespace gbhs

#endif
