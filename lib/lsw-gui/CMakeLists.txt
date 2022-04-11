
add_library(lsw-gui
        include/gui_guisystem.h
        include/gui_lighteditor.h
        include/gui_mainmenu.h
        src/gui_lighteditor.cpp
        src/gui_guisystem.cpp
        src/gui_mainmenu.cpp
        src/gui_shadereditor.cpp include/gui_shadereditor.h

        # VWR - need to be extracted to a lib independent of GUI
        include/gui_window.h
        include/gui_iwindowextension.h
        include/gui_windowinputlistener.h
        src/gui_window.cpp
        src/gui_windowinputlistener.cpp

        # IMGUI
        src/imgui_impl_glfw.h
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.h
        src/imgui_impl_opengl3.cpp include/gui_mainmenu.h src/gui_mainmenu.cpp include/gui_materialeditor.h src/gui_materialeditor.cpp
        )

target_include_directories(lsw-gui PRIVATE src PUBLIC include)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(EnTT REQUIRED)
find_package(glfw REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
target_link_libraries(lsw-gui
        PRIVATE
        izzy
        ecs
        lsw-gl
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        imgui::imgui)