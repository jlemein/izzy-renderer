
add_library(ecsw STATIC
        include/ecsw_model.h src/ecsw_model.cpp
        include/ecsw_material.h src/ecsw_material.cpp include/ecsw_transform.h include/ecsw_meshinstance.h src/ecsu_sceneutil.cpp)

target_include_directories(ecsw PUBLIC include PRIVATE src)
target_link_libraries(ecsw PRIVATE shp CONAN_PKG::glm
        PUBLIC CONAN_PKG::assimp CONAN_PKG::fmt CONAN_PKG::entt CONAN_PKG::resourcemanager)
