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
    // proxy class
    struct Array1D {
        T& operator[](const size_t& i) { return data[i]; }
        const T& operator[](const size_t& i) const { return data[i]; }
        std::shared_ptr<T[]> data = std::shared_ptr<T[]>(new T[M]);
    };

    Array1D& operator[](const size_t& i) { return data[i]; }
    const Array1D& operator[](const size_t& i) const { return data[i]; }
    std::shared_ptr<Array1D[]> data = std::shared_ptr<Array1D[]>(new Array1D[N]);
};

}  // namespace gbhs

#endif
