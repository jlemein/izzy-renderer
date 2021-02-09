add_library(vwrx STATIC src/ecs_texturesystem.cpp
        include/ecs_texturesystem.h)

target_include_directories(vwrx PUBLIC include
        PRIVATE src ${PROJECT_SOURCE_DIR}/thirdparty/stb)
target_link_libraries(vwrx PUBLIC vwr CONAN_PKG::entt ecs)