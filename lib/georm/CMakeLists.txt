
add_library(georm STATIC
        src/georm_resourcemanager.cpp
        include/georm_resourcemanager.h)

target_include_directories(georm
        PUBLIC include
        PRIVATE src)

target_link_libraries(georm
        PRIVATE
            geo
            CONAN_PKG::spdlog
            CONAN_PKG::nlohmann_json
            CONAN_PKG::resourcemanager
            CONAN_PKG::assimp
            CONAN_PKG::stb
            CONAN_PKG::fmt
        )

#if (BUILD_TESTING)
#    add_executable(geo_test src/geo_sceneloader.t.cpp include/geo_scene.h include/geo_light.h include/geo_camera.h)
#    target_link_libraries(geo_test PRIVATE geo CONAN_PKG::gtest)
#    target_include_directories(geo_test PRIVATE include)
#    #    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
#    gtest_discover_tests(geo_test)
#endif()