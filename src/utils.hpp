#ifndef EXDIMUM_UTILS_H
#define EXDIMUM_UTILS_H

#include <memory>
#include <vector>

namespace gbhs {

struct Vec2ui {
    size_t x = 0;
    size_t y = 0;
};

struct Vec2i {
    int x = 0;
    int y = 0;
};

// type, height, width
template <typename T, const unsigned int N, const unsigned int M>
struct Array2D {
    T& get(const size_t& x, const size_t& y) { return data[x + y * M]; }

   private:
    std::unique_ptr<T[]> data = std::unique_ptr<T[]>(new T[N * M]);
};

}  // namespace gbhs

#endif
