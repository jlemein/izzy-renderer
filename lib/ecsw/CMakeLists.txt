
add_library(ecsw STATIC
        include/ecsw_model.h src/ecsw_model.cpp
        include/ecsw_transform.h include/ecsw_meshinstance.h src/ecsu_sceneutil.cpp)

find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(fmt REQUIRED)
find_package(EnTT REQUIRED)

target_include_directories(ecsw PUBLIC include PRIVATE src)
target_link_libraries(ecsw PRIVATE geo glm::glm
        PUBLIC assimp::assimp fmt::fmt EnTT::EnTT)
