#add_library(camerasystem STATIC src/ecs_camerasystem.cpp include/ecs_camerasystem.h)

# already included in parent directory. Once libraries are separated this is needed.
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)

add_library(ecs STATIC include/ecs_camera.h
        include/ecs_firstpersoncontrol.h
        include/ecs_moveable.h
        include/ecs_relationship.h
        include/ecs_renderable.h
        include/ecs_texture.h
        include/ecs_transform.h
        include/ecs_transformutil.h
        include/ecs_interactable.h

        # systems
        include/ecs_transformsystem.h
        src/ecs_transformsystem.cpp
        include/ecs_texturesystem.h
        src/ecs_texturesystem.cpp
        include/ecs_camerasystem.h
        src/ecs_camerasystem.cpp
        src/ecs_firstpersoncontrolsystem.cpp

        include/ecs_firstpersoncontrolsystem.h
        include/ecs_rendersystem.h
        src/ecs_rendersystem.cpp
        src/ecs_firstpersoncontrolsystem.t.cpp src/ecs_transformutil.cpp
        include/ecs_debug.h
        include/ecs_debugsystem.h
        src/ecs_debugsystem.cpp
        include/ecs_debugshapefactory.h
        include/ecs_relationshiputil.h src/ecs_relationshiputil.cpp include/ecs_dirty.h include/ecsu_relationshipsorter.h src/ecsu_relationshipsorter.cpp src/ecs_debugshapefactory.cpp include/ecs_debugmodel.h include/ecs_name.h include/ecs_light.h include/ecs_scenegraph.h include/ecs_scenegraphentity.h ../core/include/core_exceptions.h src/ecs_scenegraph.cpp src/ecs_scenegraphentity.cpp)

target_link_libraries(ecs PUBLIC shp CONAN_PKG::entt)

message("Packages in conan: ${CONAN_PKG}")

add_executable(ecsu_test src/ecsu_relationshipsorter.t.cpp)
target_link_libraries(ecsu_test PUBLIC ecs CONAN_PKG::gtest)

#add_library(test_ecs_system STATIC
#        src/ecs_firstpersoncontrolsystem.t.cpp)

# target_link_libraries(test_ecs_system PRIVATE ecs_system gtest gtest_main)

#
#target_include_directories(camerasystem
#        PUBLIC include
#        PRIVATE src)
#
#add_library(transformsystem STATIC
#        src/ecs_transformsystem.cpp
#        include/ecs_transform.h
#        include/ecs_transformsystem.h
#        include/ecs_relationship.h
#        include/ecs_transformutil.h)
#
#target_link_libraries(camerasystem
#        PUBLIC EnTT transformsystem glm)
#
#
#add_library(viewer-textures STATIC src/ecs_texturesystem.cpp include/ecs_texturesystem.h)
#
target_include_directories(ecs
        PUBLIC include
        PRIVATE src ${PROJECT_SOURCE_DIR}/thirdparty/stb)

target_link_libraries(ecs
        PUBLIC CONAN_PKG::entt CONAN_PKG::glm inputsystem)
