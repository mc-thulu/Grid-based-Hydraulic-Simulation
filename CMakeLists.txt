cmake_minimum_required(VERSION 3.13.0)
project(exdimum VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gbhs)

find_package(GDAL CONFIG REQUIRED)

target_link_libraries(gbhs PRIVATE GDAL::GDAL)

# source files
target_sources(gbhs
    PRIVATE
        src/main.cpp
        src/manning.cpp
        src/simulation_data.cpp
        src/definitions.hpp
        src/thread_pool.cpp
)
