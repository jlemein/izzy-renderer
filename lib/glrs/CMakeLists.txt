
add_library(glrs STATIC
        include/glrs_lightsystem.h
        include/glrs_renderable.h
        include/glrs_rendersystem.h
        src/glrs_rendersystem.cpp
        src/glrs_shadersystem.cpp
        src/glrs_shadersystem.h
        src/glrs_lightsystem.cpp

)

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(glrs PUBLIC include PRIVATE src)
target_link_libraries(glrs PRIVATE ecs geo EnTT::EnTT spdlog::spdlog ecsg)