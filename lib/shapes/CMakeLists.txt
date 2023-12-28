
add_library(shp STATIC src/shp_meshloader.cpp
        include/shp_meshloader.h
        include/shp_meshtransform.h
        src/shp_meshtransform.cpp
        include/shp_primitivefactory.h
        src/shp_primitivefactory.cpp
        include/shp_mesh.h include/shp_boundingbox.h include/shp_meshinstance.h include/shp_shapeutil.h src/shp_boundingbox.cpp)

target_include_directories(shp
        PUBLIC include
        PRIVATE src)

find_package(assimp REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(shp PRIVATE
        assimp::assimp
        glm::glm)
