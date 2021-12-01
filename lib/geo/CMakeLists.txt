
add_library(geo STATIC src/geo_meshloader.cpp
        include/geo_meshloader.h
        include/geo_meshtransform.h
        include/geo_materialutil.h
        src/geo_materialutil.cpp
        src/geo_meshtransform.cpp
        include/geo_primitivefactory.h
        src/geo_primitivefactory.cpp
        include/geo_mesh.h
        include/geo_boundingbox.h include/geo_meshinstance.h include/geo_shapeutil.h src/geo_boundingbox.cpp include/geo_transform.h
        include/geo_texture.h include/geo_textureloader.h include/geo_material.h include/geo_meshutil.h src/geo_meshutil.cpp include/geo_scenetransform.h)

target_include_directories(geo
        PUBLIC include
        PRIVATE src)

find_package(nlohmann_json REQUIRED)
find_package(stb REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

target_link_libraries(geo
    PRIVATE
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
            src/geo_meshutil.t.cpp)
    target_link_libraries(geo_test PRIVATE
            glm::glm
            geo GTest::GTest nlohmann_json::nlohmann_json)
    target_include_directories(geo_test PRIVATE include)

#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
gtest_discover_tests(geo_test)
#endif()
