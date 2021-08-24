cmake_minimum_required(VERSION 3.16.3)
project(Renderer
        VERSION 1.0.0)



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

# drawback of this method is that it is always considered out-of-date, and therefore always recompiles.
#add_custom_target(compile-shaders
#        echo "Compiling shaders using GLSLC"
#        COMMAND ${CMAKE_CURRENT_LIST_DIR}/compile-shaders.sh ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders ${CMAKE_BINARY_DIR}/workspace/assets/shaders)

# a better solution is as follows:
file(GLOB SHADER_INPUT_FILES   ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders/*.vert ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders/*.frag)
file(GLOB SHADER_OUTPUT_FILES  ${CMAKE_BINARY_DIR}/workspace/assets/shaders/*.spv)
add_custom_command(
        OUTPUT ${SHADER_OUTPUT_FILES}
        COMMAND echo "Compiling shaders"
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/compile-shaders.sh ${CMAKE_CURRENT_LIST_DIR}/workspace/assets/shaders ${CMAKE_BINARY_DIR}/workspace/assets/shaders
        DEPENDS ${SHADER_INPUT_FILES})
add_custom_target(compile-shaders DEPENDS ${SHADER_OUTPUT_FILES})

# configure file will copy the file <1> to the destination <2>, only if file <1> has changed or at first run.
#configure_file(WELCOME.md $<TARGET_FILE_DIR:normalmap>/WELCOME.md COPYONLY)

# The file(...) command can use generator expressions. It makes it possible to copy (or generate) the file in the same
# location as, in this case, the normalmap target executable. Useful for unit tests.
#file(GENERATE
#        OUTPUT $<TARGET_FILE_DIR:normalmap>/WELCOME.md
#        INPUT ${CMAKE_CURRENT_LIST_DIR}/WELCOME.md)

install(FILES workspace/materials.json DESTINATION resources)
install(DIRECTORY workspace/assets DESTINATION resources)

set(CPACK_IFW_ROOT "/home/jlemein/Qt/QtIFW-4.1.1/")
set(CPACK_IFW_VERBOSE ON)
set(CPACK_GENERATOR "IFW;ZIP")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_PACKAGE_ICON "sun.png")
set(CPACK_PACKAGE_VENDOR "Lemon Software")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package-installers")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/LICENSE.md)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/README.md)
include(CPack)
include(CPackIFW)

add_subdirectory(lib)
add_subdirectory(apps)

add_dependencies(normalmap compile-shaders)

#install(FILES LICENSE.md README.md DESTINATION ${CMAKE_BINARY_DIR})

