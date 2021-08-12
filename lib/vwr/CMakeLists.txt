add_library(vwr STATIC src/vwr_viewer.cpp
        include/vwr_windowinputlistener.h
        include/vwr_viewerextension.h
        src/vwr_windowinputlistener.cpp

        include/gui_system.h
        src/gui_system.cpp
        src/imgui_impl_glfw.h
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.h
        src/imgui_impl_opengl3.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

target_include_directories(vwr PUBLIC include
        PRIVATE src)
target_link_libraries(vwr PUBLIC ecsg
        EnTT::EnTT
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        imgui::imgui
        inputsystem ecs
        georm
        )
