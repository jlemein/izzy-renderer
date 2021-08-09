
add_library(geo STATIC src/geo_meshloader.cpp
        include/geo_meshloader.h
        include/geo_meshtransform.h
        src/geo_meshtransform.cpp
        include/geo_primitivefactory.h
        src/geo_primitivefactory.cpp
        include/geo_mesh.h src/geo_mesh.cpp include/geo_boundingbox.h include/geo_meshinstance.h include/geo_shapeutil.h src/geo_boundingbox.cpp include/geo_sceneloader.h src/geo_sceneloader.cpp include/geo_transform.h
        include/geo_texture.h include/geo_textureloader.h src/geo_textureloader.cpp include/geo_material.h include/geo_ubermaterialdata.h src/geo_ubermaterialdata.cpp include/geo_meshutil.h src/geo_meshutil.cpp)

target_include_directories(geo
        PUBLIC include
        PRIVATE src)

find_package(nlohmann_json REQUIRED)
find_package(stb REQUIRED)
find_package(resourcemanager REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)

target_link_libraries(geo
        PRIVATE
            assimp::assimp
            glm::glm
            spdlog::spdlog
#            CONAN_PKG::boost
        PUBLIC
        assimp::assimp resourcemanager::resourcemanager stb::stb fmt::fmt nlohmann_json::nlohmann_json)

#if (BUILD_TESTING)
    add_executable(geo_test
            src/geo_sceneloader.t.cpp
            src/geo_material.t.cpp
            include/geo_scene.h include/geo_light.h include/geo_camera.h src/geo_meshutil.t.cpp)
    target_link_libraries(geo_test PRIVATE
            glm::glm
            geo GTest::GTest nlohmann_json::nlohmann_json)
    target_include_directories(geo_test PRIVATE include)

#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
gtest_discover_tests(geo_test)
#endif()
