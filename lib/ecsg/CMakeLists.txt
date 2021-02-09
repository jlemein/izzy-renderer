add_library(ecsg STATIC
        include/ecsg_scenegraph.h
        include/ecsg_scenegraphentity.h
        src/ecsg_scenegraph.cpp
        src/ecsg_scenegraphentity.cpp)

target_include_directories(ecsg PUBLIC include
        PRIVATE src)
target_link_libraries(ecsg PUBLIC ecs geo CONAN_PKG::resourcemanager)
