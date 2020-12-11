cmake_minimum_required(VERSION 3.16.3)
project(glviewer)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

# Assimp
include(FetchContent)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.0.1
  GIT_SHALLOW ON
)

FetchContent_Declare(
    glfw
    DOWNLOAD_DIR "thirdparty2" # The only dir option which is required
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG 3.3.2
)

FetchContent_Declare(
    glm
    DOWNLOAD_DIR "thirdparty2" # The only dir option which is required
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.8
)

FetchContent_Declare(
    entt
    DOWNLOAD_DIR "thirdparty2" # The only dir option which is required
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.5.2
)

FetchContent_Populate(assimp)
FetchContent_Populate(glfw)
FetchContent_Populate(glm)
FetchContent_Populate(entt)
add_subdirectory(${assimp_SOURCE_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${glfw_SOURCE_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${glm_SOURCE_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${entt_SOURCE_DIR} EXCLUDE_FROM_ALL)

add_subdirectory(lib)
add_subdirectory(apps)
