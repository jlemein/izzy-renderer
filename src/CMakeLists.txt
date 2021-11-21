
add_library(common INTERFACE)
target_link_libraries(common INTERFACE vwr
        workspace
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        georm
        glm::glm
        glfw::glfw)
target_include_directories(common INTERFACE .)

find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(izzyrender m_izzyrender.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(izzyrender PRIVATE common cxxopts::cxxopts)
target_compile_definitions(izzyrender PRIVATE -DVERSION_STR="1.0.0")

install(TARGETS izzyrender
        COMPONENT izzyrender
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
