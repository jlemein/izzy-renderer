
add_library(geo STATIC
        include/geo_boundingbox.h
        include/geo_camera.h
        include/geo_curve.h
        include/geo_effect.h
        include/geo_graph.h
        include/geo_light.h
        include/geo_materialtemplate.h
        include/geo_materialutil.h
        include/geo_mesh.h
        include/geo_meshinstance.h
        include/geo_meshloader.h
        include/geo_meshtransform.h
        include/geo_meshutil.h
        include/geo_primitivefactory.h
        include/geo_scene.h
        include/geo_scenetransform.h
        include/geo_shapeutil.h
        include/geo_transform.h
        src/geo_boundingbox.cpp
        src/geo_graph.cpp
#        src/geo_graph.t.cpp
#        src/geo_material.t.cpp
        src/geo_materialutil.cpp
        src/geo_meshloader.cpp
        src/geo_meshtransform.cpp
        src/geo_meshutil.cpp
#        src/geo_meshutil.t.cpp
        src/geo_primitivefactory.cpp
        src/geo_scene.cpp
       )

target_include_directories(geo
        PUBLIC include
        PRIVATE src)

find_package(nlohmann_json REQUIRED)
find_package(stb REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(gtest REQUIRED)
find_package(Boost REQUIRED)

target_link_libraries(geo
    PRIVATE
        ecs
        assimp::assimp
        glm::glm
        spdlog::spdlog
        Boost::boost
        stb::stb
        fmt::fmt
        nlohmann_json::nlohmann_json)

#if (BUILD_TESTING)
    add_executable(geo_test
            src/geo_material.t.cpp
            src/geo_meshutil.t.cpp
            src/geo_graph.t.cpp)
    target_link_libraries(geo_test PRIVATE
            glm::glm
            geo gtest::gtest nlohmann_json::nlohmann_json)
    target_include_directories(geo_test PRIVATE include)

#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
gtest_discover_tests(geo_test)
#endif()
