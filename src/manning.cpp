#include "manning.hpp"

#include <cmath>

namespace gbhs {

void Manning::step(const float& dt) {
    // TODO constants for now
    float w = 0.5f;
    float r = 0.035f;

    // in- and outflow
    l_inflow.clear();
    l_outflow.clear();
    for (const size_t& cell_idx : data.cellsWithWater()) {
        const Cell& c = data.getCell(cell_idx);

        const auto& neighbours = c.neighbours;
        if (neighbours.size() == 1) {  // TODO only one neighbour for now
            // calc flow
            float s = abs(data.cellGradient(cell_idx, neighbours[0]));
            float h = c.water_level;
            float l = data.cellDistance(cell_idx, neighbours[0]);
            float outflow = (dt / (l * w)) * w * h * (1.f / r) *
                            powf((w * h) / (w + 2.f * h), 2.f / 3.f) * sqrtf(s);
            if (outflow > h) {
                outflow = h;
            }
            l_outflow.push_back({cell_idx, -outflow});
            l_inflow.push_back({neighbours[0], outflow});
        }
    }

    // apply inflow
    for (const auto& f : l_inflow) {
        data.modifyWaterLevel(f.first, f.second);
    }

    // apply outflow
    for (const auto& f : l_outflow) {
        data.modifyWaterLevel(f.first, f.second);
    }

    // ground water & removing negative water levels
    for (const size_t& cell_idx : data.cellsWithWater()) {
        const Cell& c = data.getCell(cell_idx);
        data.setWaterLevel(cell_idx,
                           std::max(0.f, c.water_level - 0.001f * dt));
    }

    // TODO "fill_depressions"
}

}  // namespace gbhs
