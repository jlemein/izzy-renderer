
add_library(georm STATIC
        include/georm_resourcemanager.h
        include/georm_materialsystem.h
        include/anim_animationsystem.h
        include/anim_localrotation.h
        include/georm_fontsystem.h
        include/georm_exrloader.h
        include/georm_texturesystem.h
        include/georm_sceneloader.h
        include/georm_stbtextureloader.h
        src/georm_resourcemanager.cpp
        src/georm_materialsystem.cpp
        src/georm_resourcemanager.t.cpp
        src/anim_animationsystem.cpp
        src/georm_fontsystem.cpp
        src/georm_texturesystem.cpp
        src/georm_exrloader.cpp
        src/georm_sceneloader.cpp
        src/georm_stbtextureloader.cpp)

target_include_directories(georm
        PUBLIC include
        PRIVATE src)

find_package(spdlog REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenEXR REQUIRED)

target_link_libraries(georm
        PUBLIC nlohmann_json::nlohmann_json assimp::assimp
        PRIVATE
        geo ecsg
        workspace
        spdlog::spdlog
        stb::stb
        fmt::fmt
        openexr::openexr
)

#if (BUILD_TESTING)
#    add_executable(georm_test src/georm_resourcemanager.t.cpp src/georm_sceneloader.t.cpp)
#    target_link_libraries(geo_test PRIVATE georm CONAN_PKG::gtest)
#    target_include_directories(geo_test PRIVATE include)
#    #    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
#    gtest_discover_tests(geo_test)
#endif()