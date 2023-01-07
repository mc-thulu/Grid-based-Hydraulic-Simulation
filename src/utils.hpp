#ifndef EXDIMUM_UTILS_H
#define EXDIMUM_UTILS_H

#include <memory>
#include <vector>

namespace gbhs {

struct Vec2ui {
    size_t x = 0;
    size_t y = 0;
};

template <typename T>
struct Array2D {
    std::shared_ptr<T[]> data;
    size_t width = 0;
    size_t height = 0;

    Array2D() = default;
    Array2D(const Array2D& t) = delete;  // no copy constructor for now
    Array2D(const size_t& width, const size_t& height) : width(width), height(height) {
        data = std::shared_ptr<T[]>(new T[width * height]);
    }

    T& operator[](const size_t& i) { return data[i]; }
    const T& operator[](const size_t& i) const { return data[i]; }
    size_t idx(const size_t& x, const size_t& y) const { return x + y * width; }
    size_t size() const { return width * height; }
    T* ptr() { return data.get(); }
};

}  // namespace gbhs

#endif
