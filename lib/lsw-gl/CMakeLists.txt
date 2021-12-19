
add_library(lsw-gl STATIC
        include/glrs_lightsystem.h
        include/glrs_renderable.h
        include/glrs_rendersystem.h
        include/glrs_multipassframebuffer.h
        include/glrs_hdrframebuffer.h
        include/izzgl_effectsystem.h
        include/izzgl_materialsystem.h
        include/glrs_iframebuffer.h
        src/glrs_rendersystem.cpp
        src/glrs_shadersystem.cpp
        src/glrs_shadersystem.h
        src/glrs_lightsystem.cpp
        src/glrs_multipassframebuffer.cpp
        src/glrs_hdrframebuffer.cpp
        src/izzgl_effectsystem.cpp
        src/izzgl_materialsystem.cpp
        test/node.t.cpp )

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(lsw-gl PUBLIC include PRIVATE src)
target_link_libraries(lsw-gl PUBLIC izzy workspace PRIVATE ecs geo EnTT::EnTT spdlog::spdlog ecsg)