
add_library(georm STATIC
        src/georm_resourcemanager.cpp
        include/georm_resourcemanager.h
        include/georm_materialsystem.h
        src/georm_materialsystem.cpp
        src/georm_resourcemanager.t.cpp)

target_include_directories(georm
        PUBLIC include
        PRIVATE src)

target_link_libraries(georm
        PRIVATE
            geo ecsg
            CONAN_PKG::spdlog
            CONAN_PKG::nlohmann_json
            CONAN_PKG::resourcemanager
            CONAN_PKG::assimp
            CONAN_PKG::stb
            CONAN_PKG::fmt
        )

#if (BUILD_TESTING)
#    add_executable(georm_test src/georm_resourcemanager.t.cpp)
#    target_link_libraries(geo_test PRIVATE georm CONAN_PKG::gtest)
#    target_include_directories(geo_test PRIVATE include)
#    #    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
#    gtest_discover_tests(geo_test)
#endif()