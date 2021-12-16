
add_library(lsw-gl STATIC
        include/glrs_lightsystem.h
        include/glrs_renderable.h
        include/glrs_rendersystem.h
        include/glrs_multipassframebuffer.h
        include/glrs_hdrframebuffer.h
        include/glrs_effectgraph.h
        src/glrs_rendersystem.cpp
        src/glrs_shadersystem.cpp
        src/glrs_shadersystem.h
        src/glrs_lightsystem.cpp
        src/glrs_multipassframebuffer.cpp
        src/glrs_hdrframebuffer.cpp
        include/glrs_iframebuffer.h test/node.t.cpp include/glrs_effectgraph.h)

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(lsw-gl PUBLIC include PRIVATE src)
target_link_libraries(lsw-gl PRIVATE ecs geo EnTT::EnTT spdlog::spdlog ecsg)