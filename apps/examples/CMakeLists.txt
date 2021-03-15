
add_executable(example_normalmap m_normalmapping.cpp)
target_link_libraries(example_normalmap PUBLIC vwr ecs fractal fractalgenerator CONAN_PKG::entt geo
        CONAN_PKG::resourcemanager CONAN_PKG::glm CONAN_PKG::glfw)
