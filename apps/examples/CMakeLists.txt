
add_library(common INTERFACE)
target_link_libraries(common INTERFACE vwr
        workspace
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        georm
        resourcemanager::resourcemanager
        glm::glm
        glfw::glfw)
target_include_directories(common INTERFACE .)


find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(resourcemanager REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(normalmap m_normalmapping.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(normalmap PRIVATE common cxxopts::cxxopts)

add_executable(wera3d m_wera3d.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(wera3d PRIVATE common cxxopts::cxxopts)

add_executable(icosphere m_icosphere.cpp)
target_link_libraries(icosphere PRIVATE common cxxopts::cxxopts)

install(TARGETS normalmap wera3d icosphere
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
