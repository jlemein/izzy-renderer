
add_library(geo STATIC src/geo_meshloader.cpp
        include/geo_meshloader.h
        include/geo_meshtransform.h
        src/geo_meshtransform.cpp
        include/geo_primitivefactory.h
        src/geo_primitivefactory.cpp
        include/geo_mesh.h src/geo_mesh.cpp include/geo_boundingbox.h include/geo_meshinstance.h include/geo_shapeutil.h src/geo_boundingbox.cpp include/geo_sceneloader.h src/geo_sceneloader.cpp include/geo_transform.h
        include/geo_materialloader.h src/geo_materialloader.cpp
        include/geo_texture.h include/geo_textureloader.h src/geo_textureloader.cpp include/geo_material.h include/geo_ubermaterialdata.h src/geo_ubermaterialdata.cpp)

target_include_directories(geo
        PUBLIC include
        PRIVATE src)

target_link_libraries(geo PRIVATE CONAN_PKG::assimp CONAN_PKG::glm
        CONAN_PKG::spdlog
        PUBLIC CONAN_PKG::assimp CONAN_PKG::resourcemanager CONAN_PKG::stb CONAN_PKG::fmt CONAN_PKG::nlohmann_json)

#if (BUILD_TESTING)
    add_executable(geo_test src/geo_sceneloader.t.cpp include/geo_scene.h include/geo_light.h include/geo_camera.h src/geo_materialloader.t.cpp)
    target_link_libraries(geo_test PRIVATE geo CONAN_PKG::gtest CONAN_PKG::nlohmann_json)
    target_include_directories(geo_test PRIVATE include)
#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
    gtest_discover_tests(geo_test)
#endif()
