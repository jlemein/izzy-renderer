
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

add_executable(normalmap m_normalmapping.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(normalmap PRIVATE common cxxopts::cxxopts)

add_executable(parallax m_parallaxmapping.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(parallax PRIVATE common cxxopts::cxxopts)

add_executable(izzyrender m_izzyrender.cpp gui_lighteditor.h gui_lighteditor.cpp)
target_link_libraries(izzyrender PRIVATE common cxxopts::cxxopts)
target_compile_definitions(izzyrender PRIVATE -DVERSION_STR="1.0.0")

add_executable(debugscene m_debugscene.cpp)
target_link_libraries(debugscene PRIVATE common cxxopts::cxxopts)

add_executable(icosphere m_icosphere.cpp)
target_link_libraries(icosphere PRIVATE common cxxopts::cxxopts)

install(TARGETS normalmap izzyrender icosphere
        COMPONENT examples
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
install(TARGETS izzyrender
        COMPONENT izzyrender
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
