
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(cxxopts REQUIRED)

add_library(common INTERFACE)
target_link_libraries(common INTERFACE
        workspace
        ecs
        fractal
        fractalgenerator
        EnTT::EnTT geo
        glm::glm
        glfw)
target_include_directories(common INTERFACE .)

add_executable(izzyrender m_izzyrender.cpp)
target_link_libraries(izzyrender PRIVATE izzy izzgl izz-gui common cxxopts::cxxopts)
target_compile_definitions(izzyrender PRIVATE -DVERSION_STR="1.0.0")

add_executable(pb_spheres m_pbspheres.cpp)
target_link_libraries(pb_spheres PRIVATE izzy izzgl izz-gui common cxxopts::cxxopts)
target_compile_definitions(pb_spheres PRIVATE -DVERSION_STR="1.0.0")

add_executable(deferredrender m_deferredrender.cpp)
target_link_libraries(deferredrender PUBLIC izzy izzgl izz-gui common cxxopts::cxxopts)
target_compile_definitions(deferredrender PRIVATE -DVERSION_STR="1.0.0")

# Disabled: m_bloom.cpp does not build. It defines a second main() alongside
# m_deferredrender.cpp (duplicate symbol at link time) and still calls the removed
# MaterialSystem::loadMaterialsFromFile() / treats createMaterial() as returning an id.
# Source kept for reference; port it to the current MaterialSystem API to re-enable.
#add_executable(bloom m_bloom.cpp m_deferredrender.cpp)
#target_link_libraries(bloom PRIVATE izzy izz-gui common cxxopts::cxxopts)
#target_compile_definitions(bloom PRIVATE -DVERSION_STR="1.0.0")

install(TARGETS izzyrender
        COMPONENT izzyrender
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

# Executable: plane <materialtemplate_name> <width> <height>
add_executable(plane m_plane.cpp)
target_link_libraries(plane PRIVATE izzy izz-gui common cxxopts::cxxopts)
target_compile_definitions(plane PRIVATE -DVERSION_STR="0.1.0")

# -----------------------------------------------------------------------------------------
# Disabled: these examples predate the izzgl/izzgui refactor and were already unbuildable at
# commit 6d07ca1 (they are not a casualty of the conan-2.0 revert). They still #include
# <core_util.h>, which no longer exists, and m_icosphere.cpp calls a gui::Window constructor
# signature that has since changed. Sources are kept; port them to the current API to re-enable.
# -----------------------------------------------------------------------------------------
#add_executable(normalmap m_normalmapping.cpp)
#target_link_libraries(normalmap PRIVATE izzy izz-gui common cxxopts::cxxopts)
#
#add_executable(parallax m_parallaxmapping.cpp)
#target_link_libraries(parallax PRIVATE izzy izz-gui common cxxopts::cxxopts)
#
#add_executable(debugscene m_debugscene.cpp)
#target_link_libraries(debugscene PRIVATE izzy izz-gui common cxxopts::cxxopts izz-gui)
#
#add_executable(icosphere m_icosphere.cpp)
#target_link_libraries(icosphere PRIVATE izzy common cxxopts::cxxopts izz-gui)
#
#add_executable(glossyness m_glossyness.cpp)
#target_link_libraries(glossyness PRIVATE izzy izz-gui common cxxopts::cxxopts)
#target_compile_definitions(glossyness PRIVATE -DVERSION_STR="1.0.0")

install(TARGETS pb_spheres deferredrender plane
        COMPONENT examples
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
