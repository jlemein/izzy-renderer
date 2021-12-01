add_library(izzy STATIC
        include/izz_resourcemanager.h
        include/izz_materialsystem.h
        include/anim_animationsystem.h
        include/anim_localrotation.h
        include/izz_fontsystem.h
        include/izz_exrloader.h
        src/izz_resourcemanager.cpp
        src/izz_materialsystem.cpp
        src/izz_resourcemanager.t.cpp
        src/izz_animationsystem.cpp
        src/izz_fontsystem.cpp
        include/izz_texturesystem.h src/izz_texturesystem.cpp
        src/izz_exrloader.cpp
        include/izz_sceneloader.h src/izz_sceneloader.cpp
        include/izz_stbtextureloader.h src/izz_stbtextureloader.cpp
        include/izz_scenecompositionsystem.h)

target_include_directories(izzy
        PUBLIC include
        PRIVATE src)

find_package(spdlog REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
#find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenEXR REQUIRED)

target_link_libraries(izzy
        PRIVATE
        geo ecsg
        workspace
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        assimp::assimp
        stb::stb
        fmt::fmt
        OpenEXR::OpenEXR
        )

#if (BUILD_TESTING)
#    add_executable(georm_test src/georm_resourcemanager.t.cpp src/georm_sceneloader.t.cpp)
#    target_link_libraries(geo_test PRIVATE georm CONAN_PKG::gtest)
#    target_include_directories(geo_test PRIVATE include)
#    #    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
#    gtest_discover_tests(geo_test)
#endif()