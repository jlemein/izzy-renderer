
add_executable(normalmap_example m_normalmapping.cpp)
target_link_libraries(normalmap_example
        PUBLIC
            vwr
            ecs
            fractal
            fractalgenerator
            CONAN_PKG::entt geo
            georm
            CONAN_PKG::resourcemanager
            CONAN_PKG::glm
            CONAN_PKG::glfw)
