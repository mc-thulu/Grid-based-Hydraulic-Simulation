#include "manning.hpp"

#include <algorithm>
#include <cmath>

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
    simulateBlocks(dt);
    simulateBorders(dt);
    applyChanges(dt);
}

void Manning::simulateBlocks(const float& dt) {
    for (int by = 0; by < gbhs::BLOCKCNT_Y; ++by) {
        for (int bx = 0; bx < gbhs::BLOCKCNT_X; ++bx) {
            Block& b = data.blocks[by][bx];
            if (!b.containsWater()) {
                continue;
            }

            // simulate cells (ignoring the borders)
            for (int iy = 1; iy < BLOCKSIZE_Y - 1; ++iy) {
                for (int ix = 1; ix < BLOCKSIZE_X - 1; ++ix) {
                    Cell& c = b.data[iy][ix];
                    calc(c, dt);
                }
            }
        }
    }
}

void Manning::simulateBorders(const float& dt) {
    // interactions between blocks
    for (int by = 0; by < gbhs::BLOCKCNT_Y; ++by) {
        for (int bx = 0; bx < gbhs::BLOCKCNT_X; ++bx) {
            Block& b = data.blocks[by][bx];
            if (!b.containsWater()) {
                continue;
            }

            // 1. top & bottom including corners
            for (int ix = 0; ix < BLOCKSIZE_X; ++ix) {
                calc(b.data[0][ix], dt);
                calc(b.data[BLOCKSIZE_Y - 1][ix], dt);
            }

            // 2. left & right without corners
            for (int iy = 1; iy < BLOCKSIZE_Y - 1; ++iy) {
                calc(b.data[iy][0], dt);
                calc(b.data[iy][BLOCKSIZE_X - 1], dt);
            }
        }
    }
}

void Manning::applyChanges(const float& dt) {
    // apply in-/outflow & removing negative water levels
    // TODO less work
    for (int by = 0; by < gbhs::BLOCKCNT_Y; ++by) {
        for (int bx = 0; bx < gbhs::BLOCKCNT_X; ++bx) {
            Block& b = data.blocks[by][bx];
            if (!b.containsWater()) {
                continue;
            }

            for (int iy = 0; iy < BLOCKSIZE_Y; ++iy) {
                for (int ix = 0; ix < BLOCKSIZE_X; ++ix) {
                    Cell& c = b.data[iy][ix];
                    if (c.water_level <= 0.f && c.water_level_change <= 0.f) {
                        continue;
                    }

                    c.water_level = std::max(
                        0.f,
                        c.water_level + c.water_level_change - 0.001f * dt + c.rain / 10);
                    c.water_level_change = 0.0f;
                    b.cells_with_water.set(ix + iy * BLOCKSIZE_X,
                                           c.water_level > 0.0f || c.rain > 0.0f);
                }
            }
        }
    }
}

}  // namespace gbhs
