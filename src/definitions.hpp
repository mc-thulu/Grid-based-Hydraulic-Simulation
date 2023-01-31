#ifndef GBHS_DEFINITIONS_H
#define GBHS_DEFINITIONS_H

namespace gbhs {
namespace constants {

// dataset settings
constexpr int32_t DATA_OFFSET_X = 0;
constexpr int32_t DATA_OFFSET_Y = 0;
constexpr int WORLDSIZE_X = 23558;  // TODO dynamic
constexpr int WORLDSIZE_Y = 20000;  // TODO dynamic

// block settings
constexpr int BLOCKSIZE_X = 250;
constexpr int BLOCKSIZE_Y = 250;
constexpr int BLOCKSIZE = BLOCKSIZE_X * BLOCKSIZE_Y;
constexpr int BLOCKCNT_X = (WORLDSIZE_X / BLOCKSIZE_X) + 1;
constexpr int BLOCKCNT_Y = (WORLDSIZE_Y / BLOCKSIZE_Y) + 1;
constexpr int BLOCKCNT = BLOCKCNT_X * BLOCKCNT_Y;

// simulation settings
constexpr size_t SIMULATION_STEPS = 1000;
constexpr float DT = 0.1f;                 // [sec]
constexpr size_t OUTPUT_RESOLUTION = 150;  // [steps]
constexpr int PERLIN_NOISE_SCALE = 4000;
constexpr unsigned int PERLIN_NOISE_SEED = 123456u;

// rain settings
constexpr double RAINFALL_VOLUME = 20;  // [liter per hour per m^2]
constexpr double RAINFALL_HEIGHT =
    RAINFALL_VOLUME * 2.777e-7;  // [meter per second per m^2]
constexpr double GROUND_INFILTRATION = 2.777e-7;  // [meter per second per m^2]

}  // namespace constants
}  // namespace gbhs

#endif
