#include "manning.hpp"

#include <algorithm>
#include <cmath>

#include "definitions.hpp"

using namespace gbhs::constants;

namespace gbhs {

void Manning::calc(Cell& c, const float dt) {
    float w = 0.5f;
    float r = 0.035f;
    if (c.neighbor != nullptr && c.water_level > 0.f) {
        // calc flow
        float s = c.gradient;
        float h = c.water_level;
        float l = c.distance;
        float outflow = (dt / (l * w)) * w * h * (1.f / r) *
                        powf((w * h) / (w + 2.f * h), 2.f / 3.f) * sqrtf(s);
        if (outflow > h) {
            outflow = h;
        }
        c.water_level -= outflow;
        c.neighbor->water_level_change += outflow;
    }
}

void Manning::step(const float& dt) {
    // in- and outflow
    for (int i = 0; i < BLOCKCNT_Y; ++i) {
        tp.addTask([this, dt, i]() { simulateBlocks(dt, i); });
    }
    tp.waitUntilDone();
    simulateBorders(dt);

    // apply changes
    for (int i = 0; i < BLOCKCNT_Y; ++i) {
        tp.addTask([this, dt, i]() { applyChanges(dt, i); });
    }
    tp.waitUntilDone();
}

void Manning::simulateBlocks(float dt, int row) {
    // for (int by = n; by < m; ++by) {
    for (int bx = 0; bx < BLOCKCNT_X; ++bx) {
        Block& b = data.blocks.get(bx, row);
        if (!b.containsWater()) {
            continue;
        }

        // simulate cells (ignoring the borders)
        for (int iy = 1; iy < BLOCKSIZE_Y - 1; ++iy) {
            for (int ix = 1; ix < BLOCKSIZE_X - 1; ++ix) {
                Cell& c = b.data.get(ix, iy);
                calc(c, dt);
            }
        }
    }
}

void Manning::simulateBorders(float dt) {
    // interactions between blocks
    for (int by = 0; by < BLOCKCNT_Y; ++by) {
        for (int bx = 0; bx < BLOCKCNT_X; ++bx) {
            Block& b = data.blocks.get(bx, by);
            if (!b.containsWater()) {
                continue;
            }

            // 1. top & bottom including corners
            for (int ix = 0; ix < BLOCKSIZE_X; ++ix) {
                calc(b.data.get(ix, 0), dt);
                calc(b.data.get(ix, BLOCKSIZE_Y - 1), dt);
            }

            // 2. left & right without corners
            for (int iy = 1; iy < BLOCKSIZE_Y - 1; ++iy) {
                calc(b.data.get(0, iy), dt);
                calc(b.data.get(BLOCKSIZE_X - 1, iy), dt);
            } 
        }
    }
}

void Manning::applyChanges(float dt, int row) {
    // apply in-/outflow & removing negative water levels
    // TODO less work
    // for (int by = 0; by < BLOCKCNT_Y; ++by) {
    int by = row;
    for (int bx = 0; bx < BLOCKCNT_X; ++bx) {
        Block& b = data.blocks.get(bx, by);
        if (!b.containsWater()) {
            continue;
        }

        for (int iy = 0; iy < BLOCKSIZE_Y; ++iy) {
            for (int ix = 0; ix < BLOCKSIZE_X; ++ix) {
                Cell& c = b.data.get(ix, iy);
                if (c.water_level <= 0.f && c.water_level_change <= 0.f &&
                    c.rain <= 0.0f) {
                    continue;
                }
                // TODO: floating point precision!
                c.water_level = std::max(
                    0.f,
                    c.water_level + c.water_level_change -
                        static_cast<float>(GROUND_INFILTRATION) * dt + c.rain * dt);
                c.water_level_change = 0.0f;
                b.cells_with_water.set(ix + iy * BLOCKSIZE_X,
                                       c.water_level > 0.0f || c.rain > 0.0f);
            }
        }
    }
    // }
}

}  // namespace gbhs
