add_library(vwr STATIC src/vwr_viewer.cpp
        include/vwr_windowinputlistener.h
        include/vwr_viewerextension.h
        src/vwr_windowinputlistener.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
target_include_directories(vwr PUBLIC include
        PRIVATE src)
target_link_libraries(vwr PUBLIC ecsg
        EnTT::EnTT
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        inputsystem ecs
        )
