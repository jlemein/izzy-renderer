cmake_minimum_required(VERSION 3.16.3)
project(Renderer)

include(CTest)
enable_testing()
include(GoogleTest)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(conan)
conan_cmake_run(CONANFILE conanfile.py
        BASIC_SETUP
        GENERATORS CMakeDeps)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        )
list (APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_STANDARD 17)

#set(SPIRV_INPUT_FILES
#        assets/shaders/shader.frag
#        assets/shaders/diffuse.frag
#        assets/shaders/shader.vert
#        assets/shaders/diffuse.vert)
#
#set(SPIRV_OUTPUT_FILES
#        ${CMAKE_BINARY_DIR}/assets/shaders/shader.frag.spv
#        ${CMAKE_BINARY_DIR}/assets/shaders/shader.vert.spv
#        ${CMAKE_BINARY_DIR}/assets/shaders/diffuse.frag.spv
#        ${CMAKE_BINARY_DIR}/assets/shaders/diffuse.vert.spv)

#add_custom_command(OUTPUT ${SPIRV_OUTPUT_FILES}
#        COMMAND sh ./compile-shaders.sh "${CMAKE_BINARY_DIR}"
#        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#        DEPENDS ${SPIRV_INPUT_FILES})

add_custom_target(compile-shaders ALL
        COMMAND sh ./compile-shaders.sh "${CMAKE_BINARY_DIR}" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_custom_target(copy-config-files ALL
        COMMAND sh ./compile-shaders.sh "${CMAKE_BINARY_DIR}" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})




include(FetchContent)

add_subdirectory(lib)
add_subdirectory(apps)
