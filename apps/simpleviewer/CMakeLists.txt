
add_custom_target(copy-assets ALL
        COMMAND cmake -E copy "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
        COMMAND cmake -E copy "${PROJECT_SOURCE_DIR}/testassets" "${CMAKE_BINARY_DIR}/testassets")

add_executable(simpleviewer-app src/main.cpp include/ecs_factories.h src/ecs_factories.cpp include/ecsw_camera.h include/scenegraph.h include/gui_system.h src/gui_system.cpp src/imgui_impl_glfw.h src/imgui_impl_glfw.cpp src/imgui_impl_opengl3.h src/imgui_impl_opengl3.cpp)
target_link_libraries(simpleviewer-app PUBLIC vwr ecs fractal fractalgenerator CONAN_PKG::entt geo
        CONAN_PKG::resourcemanager CONAN_PKG::glm CONAN_PKG::imgui CONAN_PKG::glfw)
target_include_directories(simpleviewer-app PUBLIC include PRIVATE src)
