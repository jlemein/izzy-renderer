
add_library(lsw-gui
        include/gui_guisystem.h
        include/gui_lighteditor.h
        src/gui_lighteditor.cpp
        src/gui_guisystem.cpp

        # VWR - need to be extracted to a lib independent of GUI
        include/vwr_viewer.h
        include/vwr_viewerextension.h
        include/vwr_windowinputlistener.h
        src/vwr_viewer.cpp
        src/vwr_windowinputlistener.cpp

        # IMGUI
        src/imgui_impl_glfw.h
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.h
        src/imgui_impl_opengl3.cpp)

target_include_directories(lsw-gui PRIVATE src PUBLIC include)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
target_link_libraries(lsw-gui
        PRIVATE
        ecsg georm
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        imgui::imgui)