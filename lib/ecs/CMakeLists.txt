#add_library(camerasystem STATIC src/ecs_camerasystem.cpp include/ecs_camerasystem.h)

# already included in parent directory. Once libraries are separated this is needed.
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)

set (HEADERS
        include/uniform_parallax.h
        include/uniform_ubermaterial.h
        include/uniform_lambert.h
        src/uniform_ubermaterial.cpp include/uniform_uniformblockmanager.h
        include/ecs_wireframe.h include/ecs_postprocess.h)

add_library(ecs STATIC include/ecs_camera.h
        include/ecs_firstpersoncontrol.h
        include/ecs_moveable.h
        include/ecs_relationship.h
        include/ecs_texture.h
        include/ecs_transform.h
        include/ecs_transformutil.h

        # systems
        include/ecs_transformsystem.h
        src/ecs_transformsystem.cpp
        include/ecs_camerasystem.h
        src/ecs_camerasystem.cpp
        src/ecs_firstpersoncontrolsystem.cpp

        include/ecs_firstpersoncontrolsystem.h
        src/ecs_transformutil.cpp
        include/ecs_debug.h
        include/ecs_debugsystem.h
        include/ecs_relationshiputil.h src/ecs_relationshiputil.cpp include/ecs_dirty.h
        include/ecsu_relationshipsorter.h src/ecsu_relationshipsorter.cpp
        include/ecs_name.h include/ecs_light.h
        include/core_util.h
        ${HEADERS})

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(ecs PUBLIC include PRIVATE src)
target_link_libraries(ecs PUBLIC geo EnTT::EnTT spdlog::spdlog)

find_package(GTest REQUIRED)
add_executable(ecsu_test src/ecsu_relationshipsorter.t.cpp)

target_link_libraries(ecsu_test PUBLIC ecs GTest::GTest)


add_executable(ecs_test
        src/ecs_shader.t.cpp
        src/ecs_firstpersoncontrolsystem.t.cpp
        src/ecs_transformsystem.t.cpp
        )
target_link_libraries(ecs_test PRIVATE ecs GTest::GTest)
target_include_directories(ecs_test PRIVATE src)

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
#
target_include_directories(ecs
        PUBLIC include
        PRIVATE src)

find_package(EnTT REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(ecs
        PUBLIC EnTT::EnTT glm::glm inputsystem)

