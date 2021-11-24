
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(normalmap m_normalmapping.cpp)
target_link_libraries(normalmap PRIVATE lsw-gui common cxxopts::cxxopts)

add_executable(parallax m_parallaxmapping.cpp)
target_link_libraries(parallax PRIVATE lsw-gui common cxxopts::cxxopts)

add_executable(debugscene m_debugscene.cpp)
target_link_libraries(debugscene PRIVATE lsw-gui common cxxopts::cxxopts lsw-gui)

add_executable(icosphere m_icosphere.cpp)
target_link_libraries(icosphere PRIVATE common cxxopts::cxxopts lsw-gui)

install(TARGETS normalmap icosphere
        COMPONENT examples
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
