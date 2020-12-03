cmake_minimum_required(VERSION 3.16.3)
project(glviewer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)

set(SPIRV_INPUT_FILES
        assets/shaders/shader.frag
        assets/shaders/diffuse.frag
        assets/shaders/shader.vert
        assets/shaders/diffuse.vert)

set(SPIRV_OUTPUT_FILES
        ${CMAKE_BINARY_DIR}/assets/shaders/shader.frag.spv
        ${CMAKE_BINARY_DIR}/assets/shaders/shader.vert.spv
        ${CMAKE_BINARY_DIR}/assets/shaders/diffuse.frag.spv
        ${CMAKE_BINARY_DIR}/assets/shaders/diffuse.vert.spv)

add_custom_command(OUTPUT ${SPIRV_OUTPUT_FILES}
        COMMAND sh ./compile-shaders.sh "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${SPIRV_INPUT_FILES})

add_custom_target(compile-shaders ALL DEPENDS ${SPIRV_OUTPUT_FILES})

set(ASSIMP_BUILD_TESTS OFF)
set(BUILD_TESTING OFF)
add_subdirectory(thirdparty/assimp EXCLUDE_FROM_ALL)
# add_subdirectory(thirdparty/googletest EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/glfw-3.3.2 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/glm EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/entt EXCLUDE_FROM_ALL)
#add_subdirectory(thirdparty/OIS EXCLUDE_FROM_ALL)
#add_subdirectory(thirdparty/gainput EXCLUDE_FROM_ALL)

add_subdirectory(lib)
add_subdirectory(apps)
