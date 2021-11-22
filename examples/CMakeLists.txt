
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(normalmap m_normalmapping.cpp ../src/gui_lighteditor.h ../src/gui_lighteditor.cpp)
target_link_libraries(normalmap PRIVATE common cxxopts::cxxopts)

add_executable(parallax m_parallaxmapping.cpp ../src/gui_lighteditor.h ../src/gui_lighteditor.cpp)
target_link_libraries(parallax PRIVATE common cxxopts::cxxopts)

add_executable(debugscene m_debugscene.cpp ../src/gui_lighteditor.h ../src/gui_lighteditor.cpp)
target_link_libraries(debugscene PRIVATE common cxxopts::cxxopts)

add_executable(icosphere m_icosphere.cpp)
target_link_libraries(icosphere PRIVATE common cxxopts::cxxopts)

install(TARGETS normalmap icosphere
        COMPONENT examples
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
