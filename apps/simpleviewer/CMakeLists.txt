
file(COPY ${PROJECT_SOURCE_DIR}/assets/models DESTINATION ${CMAKE_BINARY_DIR}/assets)
file(COPY ${PROJECT_SOURCE_DIR}/assets/textures DESTINATION ${CMAKE_BINARY_DIR}/assets/textures)

add_executable(simpleviewer-app src/main.cpp include/ecs_factories.h src/ecs_factories.cpp include/ecsw_camera.h)
target_link_libraries(simpleviewer-app PUBLIC ecs_system simpleviewer fractal fractalgenerator EnTT shp)
target_include_directories(simpleviewer-app PUBLIC include PRIVATE src)
