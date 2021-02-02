
add_library(shp STATIC src/geo_meshloader.cpp
        include/geo_meshloader.h
        include/geo_meshtransform.h
        src/geo_meshtransform.cpp
        include/geo_primitivefactory.h
        src/geo_primitivefactory.cpp
        include/geo_mesh.h src/geo_mesh.cpp include/geo_boundingbox.h include/geo_meshinstance.h include/geo_shapeutil.h src/geo_boundingbox.cpp include/geo_sceneloader.h src/geo_sceneloader.cpp include/geo_transform.h)

target_include_directories(shp
        PUBLIC include
        PRIVATE src)

target_link_libraries(shp PRIVATE CONAN_PKG::assimp CONAN_PKG::glm
        CONAN_PKG::spdlog
        PUBLIC CONAN_PKG::assimp CONAN_PKG::resourcemanager)

if (BUILD_TESTING)
    add_executable(geo_test src/geo_sceneloader.t.cpp include/geo_scene.h)
    target_link_libraries(geo_test PRIVATE shp CONAN_PKG::gtest)
    target_include_directories(geo_test PRIVATE include)
#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
    gtest_discover_tests(geo_test)
endif()
