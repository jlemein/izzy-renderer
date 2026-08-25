
add_library(geo STATIC
        ../../include/izz_boundingbox.h
        include/geo_camera.h
        include/geo_curve.h
        include/geo_effect.h
        include/geo_graph.h
        include/geo_light.h
        include/geo_materialutil.h
        include/geo_mesh.h
        include/geo_meshinstance.h
        include/geo_meshtransform.h
        include/geo_meshutil.h
        include/geo_primitivefactory.h
        include/geo_scene.h
        include/geo_scenetransform.h
        include/geo_shapeutil.h
        include/geo_transform.h
        src/geo_graph.cpp
#        src/geo_graph.t.cpp
#        src/geo_material.t.cpp
        src/geo_meshtransform.cpp
        src/geo_meshutil.cpp
#        src/geo_meshutil.t.cpp
        src/geo_primitivefactory.cpp
        src/geo_scene.cpp
       )

target_include_directories(geo
        # geo_mesh.h includes <izz_boundingbox.h> and geo_effect.h includes
        # <izzgl_material.h>, which live in the top-level include/ and lib/izzgl/include.
        # geo cannot link those targets (izzy/izzgl link geo), so add the dirs directly.
        PUBLIC include
               ${PROJECT_SOURCE_DIR}/include
               ${PROJECT_SOURCE_DIR}/lib/izzgl/include
        PRIVATE src)

find_package(nlohmann_json REQUIRED)
find_package(stb REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

target_link_libraries(geo
    PUBLIC
        # geo_mesh.h (a public header) includes <boost/functional/hash.hpp> and glm.
        # Under conan 1 conan_basic_setup() put every include dir on every target;
        # with conan 2 CMakeDeps these must propagate through the link interface.
        Boost::boost
        glm::glm
        # geo_graph.h (public header) includes <fmt/format.h>
        fmt::fmt
    PRIVATE
        ecs
        assimp::assimp
        spdlog::spdlog
        stb::stb
        nlohmann_json::nlohmann_json)

#if (BUILD_TESTING)
    add_executable(geo_test
            # src/geo_material.t.cpp -- stale: asserts on Material::vertexShader, a member that
            # no longer exists on izz::gl::Material. Re-enable once ported to MaterialTemplate.
            src/geo_meshutil.t.cpp
            src/geo_graph.t.cpp)
    find_package(EnTT REQUIRED)
    # geo_material.t.cpp exercises izz::gl::Material, so geo_test needs the full izzgl
    # interface (entt, spdlog, ...), not just geo. Linking an executable against izzgl is
    # fine even though izzgl itself links geo.
    target_link_libraries(geo_test PRIVATE
            glm::glm
            EnTT::EnTT
            izzgl
            geo GTest::gtest GTest::gtest_main nlohmann_json::nlohmann_json)
    # geo_material.t.cpp pulls in izzgl_material.h -> gl_renderable.h -> ecs_texture.h
    target_include_directories(geo_test PRIVATE include ${PROJECT_SOURCE_DIR}/lib/ecs/include)

#    set_target_properties(geo_test PROPERTIES WORKING_DIRECTORY ./testdata)
gtest_discover_tests(geo_test)
#endif()
