add_library(ecsg STATIC
        include/ecsg_scenegraph.h
        include/ecsg_scenegraphentity.h
        src/ecsg_scenegraph.cpp
        src/ecsg_scenegraphentity.cpp include/ecsg_lightfactory.h src/ecsg_lightfactory.cpp)

target_include_directories(ecsg PUBLIC include
        PRIVATE src)
target_link_libraries(ecsg PUBLIC ecs glrs geo)
