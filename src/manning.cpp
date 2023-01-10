#include "manning.hpp"

#include <algorithm>
#include <cmath>

namespace gbhs {

void Manning::fillDepressions() {
    // "fill_depressions"
    std::sort(data.cellsWithWater().begin(),
              data.cellsWithWater().end(),
              [&](const size_t& idx_1, const size_t& idx_2) {
                  return data.height_map[idx_1] < data.height_map[idx_2];
              });

    for (const size_t& cell_idx : data.cellsWithWater()) {
        const Cell& c = data.getCell(cell_idx);
        float cell_height = data.height_map[cell_idx];

        for (size_t i = 0; i < c.higher_neigbours.size(); ++i) {
            const Cell& neighbor = data.getCell(c.higher_neigbours[i]);
            float neighbor_height = data.height_map[c.higher_neigbours[i]];
            if (cell_height + c.water_level <= neighbor_height) {
                break;
            }

            // fill neighbor
            float level_balance =
                (cell_height + c.water_level - neighbor_height) / (2 + i);

            // select the next highest neighbor
            if (i + 1 < c.higher_neigbours.size()) {
                float next_neighbor_height = data.height_map[c.higher_neigbours[i + 1]];
                level_balance =
                    std::min(level_balance, next_neighbor_height - neighbor_height);
            }

            data.modifyWaterLevel(cell_idx, -level_balance * (1 + i));

            for (size_t m = 0; m < i + 1; ++m) {
                data.modifyWaterLevel(c.higher_neigbours[m], level_balance);
            }
        }
    }
}

void Manning::step(const float& dt) {
    // TODO constants for now
    float w = 0.5f;
    float r = 0.035f;

    // in- and outflow
    for (const size_t& cell_idx : data.cellsWithWater()) {
        Cell& c = data.getCell(cell_idx);

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
            c.water_level_change -= outflow;
            data.getCell(neighbours[0]).water_level_change += outflow;
        }
    }

    // apply in-/outflow & removing negative water levels
    for (const size_t& cell_idx : data.cellsWithWater()) {
        Cell& c = data.getCell(cell_idx);
        data.setWaterLevel(
            cell_idx, std::max(0.f, c.water_level + c.water_level_change - 0.001f * dt));
        c.water_level_change = 0.0f;
    }

    fillDepressions();
}

}  // namespace gbhs
