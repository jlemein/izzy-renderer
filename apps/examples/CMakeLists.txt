
add_executable(normalmap_example m_normalmapping.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_include_directories(normalmap_example PRIVATE .)

find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(resourcemanager REQUIRED)
find_package(cxxopts REQUIRED)
target_link_libraries(normalmap_example
        PUBLIC
        vwr
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        georm
        resourcemanager::resourcemanager
        glm::glm
        glfw::glfw
        cxxopts::cxxopts)

add_executable(wera3d m_wera3d.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_include_directories(wera3d PRIVATE .)
target_link_libraries(wera3d
        PUBLIC
        vwr
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        georm
        resourcemanager::resourcemanager
        glm::glm
        glfw::glfw
        cxxopts::cxxopts)

install(TARGETS normalmap_example wera3d
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
