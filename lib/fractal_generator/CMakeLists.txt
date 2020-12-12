
add_library(fractalgenerator STATIC
        include/fractal_generator.h
        src/fractal_generator.cpp)

target_include_directories(fractalgenerator
        PUBLIC include
        PRIVATE src)

target_link_libraries(fractalgenerator PRIVATE fractal simpleviewer CONAN_PKG::entt shp)
