
add_library(lsw-gl STATIC
        include/ecs_debugmodel.h
        include/ecs_debugshapefactory.h
        include/glrs_lightsystem.h
        include/gl_renderable.h
        include/gl_rendersystem.h
        include/gl_multipassframebuffer.h
        include/gl_hdrframebuffer.h
        include/izzgl_effectsystem.h
        include/izzgl_materialsystem.h
        include/gl_iframebuffer.h
        src/ecs_debugsystem.cpp
        src/ecs_debugshapefactory.cpp
        src/gl_rendersystem.cpp
        src/gl_shadersystem.cpp
        src/gl_shadersystem.h
        src/glrs_lightsystem.cpp
        src/gl_multipassframebuffer.cpp
        src/gl_hdrframebuffer.cpp
        src/izzgl_effectsystem.cpp
        src/izzgl_materialsystem.cpp
        include/gl_forwardrenderer.h
        include/gl_deferredrenderer.h
        src/gl_forwardrenderer.cpp
        src/gl_deferredrenderer.cpp include/gl_common.h src/gl_common.cpp
        src/gl_deferredrenderablefactory.cpp
        include/gl_deferredrenderablefactory.h
        include/gl_renderutils.h src/gl_renderutils.cpp)
#test/node.t.cpp

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(lsw-gl PUBLIC include PRIVATE src)
target_link_libraries(lsw-gl PUBLIC izzy workspace ecs geo EnTT::EnTT spdlog::spdlog)


find_package(GTest REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
add_executable(lsw-gl-test test/deferredrendersystem.t.cpp)
target_link_libraries(lsw-gl-test PUBLIC gtest::gtest lsw-gl geo OpenGL::GL GLEW::GLEW)