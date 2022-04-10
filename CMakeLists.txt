cmake_minimum_required(VERSION 3.20.0)
project(Renderer
        VERSION 0.1.0)

include(CTest)
enable_testing()
include(GoogleTest)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(conan)
conan_cmake_run(CONANFILE conanfile.py BASIC_SETUP)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        )
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUSE_DEFERRED_RENDERING)
add_library(izzy STATIC
        include/izz_resourcemanager.h
        lib/lsw-gl/include/izzgl_materialsystem.h
        include/anim_animationsystem.h
        include/anim_localrotation.h
        include/izz_fontsystem.h
        include/izz_exrloader.h
        src/izz_resourcemanager.cpp
        lib/lsw-gl/src/izzgl_materialsystem.cpp
        src/izz_resourcemanager.t.cpp
        src/izz_animationsystem.cpp
        src/izz_fontsystem.cpp
        include/izz_texturesystem.h src/izz_texturesystem.cpp
        src/izz_exrloader.cpp
        include/izz_scenegraphhelper.h
        include/izz_scenegraphentity.h
        src/izz_scenegraphhelper.cpp
        src/izz_scenegraphentity.cpp
        include/izz_sceneloader.h src/izz_sceneloader.cpp
        include/izz_stbtextureloader.h src/izz_stbtextureloader.cpp
        include/izz_scenecompositionsystem.h lib/lsw-gl/include/izzgl_effectsystem.h include/izz_renderablecomponentfactory.h)

target_include_directories(izzy
        PUBLIC include
        PRIVATE src)

find_package(spdlog REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
#find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenEXR REQUIRED)

target_link_libraries(izzy
        PRIVATE
        geo ecs lsw-gl
        workspace
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        assimp::assimp
        stb::stb
        fmt::fmt
        openexr::openexr
        )

#if (BUILD_TESTING)
#    add_executable(georm_test src/georm_resourcemanager.t.cpp src/georm_sceneloader.t.cpp)
#    target_link_libraries(geo_test PRIVATE georm CONAN_PKG::gtest)
#    target_include_directories(geo_test PRIVATE include)
#    #    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
#    gtest_discover_tests(geo_test)
#endif()

# drawback of this method is that it is always considered out-of-date, and therefore always recompiles.
#add_custom_target(compile-shaders
#        echo "Compiling shaders using GLSLC"
#        COMMAND ${CMAKE_CURRENT_LIST_DIR}/compile-shaders.sh ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders ${CMAKE_BINARY_DIR}/workspace/assets/shaders)

# a better solution is as follows:
#file(GLOB SHADER_INPUT_FILES   ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders/*.vert ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders/*.frag)
#file(GLOB SHADER_OUTPUT_FILES  ${CMAKE_BINARY_DIR}/resources/shaders/*.spv)
#add_custom_command(
#        OUTPUT ${SHADER_OUTPUT_FILES}
#        COMMAND echo "Compiling shaders"
#        COMMAND ${CMAKE_CURRENT_LIST_DIR}/compile-shaders.sh ${CMAKE_CURRENT_LIST_DIR}/resources/shaders ${CMAKE_BINARY_DIR}/resources/shaders
#        DEPENDS ${SHADER_INPUT_FILES})
#add_custom_target(compile-shaders ALL DEPENDS ${SHADER_OUTPUT_FILES})

####################################################################################
### Resources
####################################################################################
install(FILES ${SHADER_OUTPUT_FILES} DESTINATION resources/shaders)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION .)
install(FILES ${CMAKE_SOURCE_DIR}/compile-shaders.sh DESTINATION tools)

configure_file(${CMAKE_SOURCE_DIR}/resources/materials.json ${CMAKE_BINARY_DIR}/resources/materials.json COPYONLY)
file(COPY resources/textures DESTINATION ${CMAKE_BINARY_DIR}/resources)

# configure file will copy the file <1> to the destination <2>, only if file <1> has changed or at first run.
#configure_file(WELCOME.md $<TARGET_FILE_DIR:normalmap>/WELCOME.md COPYONLY)

# The file(...) command can use generator expressions. It makes it possible to copy (or generate) the file in the same
# location as, in this case, the normalmap target executable. Useful for unit tests.
#file(GENERATE
#        OUTPUT $<TARGET_FILE_DIR:normalmap>/WELCOME.md
#        INPUT ${CMAKE_CURRENT_LIST_DIR}/WELCOME.md)

add_subdirectory(lib)
add_subdirectory(examples)

#add_dependencies(normalmap compile-shaders)

#install(FILES LICENSE.md README.md DESTINATION ${CMAKE_BINARY_DIR})



