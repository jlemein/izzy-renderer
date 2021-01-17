
file(COPY ${PROJECT_SOURCE_DIR}/assets/models DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY ${PROJECT_SOURCE_DIR}/assets/textures DESTINATION ${CMAKE_BINARY_DIR}/assets/textures)

add_executable(simpleviewer-app src/main.cpp include/ecs_factories.h src/ecs_factories.cpp include/ecsw_camera.h)
target_link_libraries(simpleviewer-app PUBLIC ecs simpleviewer fractal fractalgenerator CONAN_PKG::entt shp
        CONAN_PKG::resourcemanager)
target_include_directories(simpleviewer-app PUBLIC include PRIVATE src)
