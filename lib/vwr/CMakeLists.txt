add_library(vwr STATIC src/vwr_viewer.cpp
        include/vwr_windowinputlistener.h
        include/vwr_viewerextension.h
        src/vwr_windowinputlistener.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
target_include_directories(vwr PUBLIC include
        PRIVATE src)
target_link_libraries(vwr PUBLIC ecsg CONAN_PKG::entt CONAN_PKG::glfw OpenGL::GL GLEW::GLEW CONAN_PKG::glm inputsystem ecs
        )
