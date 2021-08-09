
add_custom_target(copy-assets ALL
        COMMAND cmake -E copy "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
        COMMAND cmake -E copy "${PROJECT_SOURCE_DIR}/testassets" "${CMAKE_BINARY_DIR}/testassets")

find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(resourcemanager REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

add_executable(simpleviewer-app src/main.cpp include/ecs_factories.h src/ecs_factories.cpp include/ecsw_camera.h include/scenegraph.h include/gui_system.h src/gui_system.cpp src/imgui_impl_glfw.h src/imgui_impl_glfw.cpp src/imgui_impl_opengl3.h src/imgui_impl_opengl3.cpp)

target_link_libraries(simpleviewer-app PUBLIC vwr ecs fractal fractalgenerator EnTT::EnTT geo georm
        resourcemanager::resourcemanager
        glm::glm imgui::imgui glfw::glfw)

target_include_directories(simpleviewer-app PUBLIC include PRIVATE src)
