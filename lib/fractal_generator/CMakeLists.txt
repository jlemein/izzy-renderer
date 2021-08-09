
add_library(fractalgenerator STATIC
        include/fractal_generator.h
        src/fractal_generator.cpp)

target_include_directories(fractalgenerator
        PUBLIC include
        PRIVATE src)

find_package(EnTT REQUIRED)
target_link_libraries(fractalgenerator PRIVATE fractal EnTT::EnTT geo)
