
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw REQUIRED)
find_package(cxxopts REQUIRED)

add_library(common INTERFACE)
target_link_libraries(common INTERFACE
        workspace
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        glm::glm
        glfw::glfw)
target_include_directories(common INTERFACE .)

add_executable(izzyrender m_izzyrender.cpp)
target_link_libraries(izzyrender PRIVATE izzy lsw-gl lsw-gui common cxxopts::cxxopts)
target_compile_definitions(izzyrender PRIVATE -DVERSION_STR="1.0.0")

add_executable(deferredrender m_deferredrender.cpp)
target_link_libraries(deferredrender PUBLIC izzy lsw-gl lsw-gui common cxxopts::cxxopts)
target_compile_definitions(deferredrender PRIVATE -DVERSION_STR="1.0.0")

add_executable(bloom m_bloom.cpp m_deferredrender.cpp)
target_link_libraries(bloom PRIVATE izzy lsw-gui common cxxopts::cxxopts)
target_compile_definitions(bloom PRIVATE -DVERSION_STR="1.0.0")

install(TARGETS izzyrender
        COMPONENT izzyrender
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

add_executable(normalmap m_normalmapping.cpp)
target_link_libraries(normalmap PRIVATE izzy lsw-gui common cxxopts::cxxopts)

add_executable(parallax m_parallaxmapping.cpp)
target_link_libraries(parallax PRIVATE izzy lsw-gui common cxxopts::cxxopts)

add_executable(debugscene m_debugscene.cpp)
target_link_libraries(debugscene PRIVATE izzy lsw-gui common cxxopts::cxxopts lsw-gui)

add_executable(icosphere m_icosphere.cpp)
target_link_libraries(icosphere PRIVATE izzy common cxxopts::cxxopts lsw-gui)

add_executable(glossyness m_glossyness.cpp)
target_link_libraries(glossyness PRIVATE izzy lsw-gui common cxxopts::cxxopts)
target_compile_definitions(glossyness PRIVATE -DVERSION_STR="1.0.0")

install(TARGETS normalmap icosphere
        COMPONENT examples
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
