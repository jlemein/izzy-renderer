
add_library(izz-gui
        include/izzgui_guisystem.h
        include/izzgui_mainmenu.h
        include/izzgui_materialeditor.h
        include/izzgui_materialinspector.h
        include/izzgui_scenenavigator.h
        include/izzgui_stats.h src/izzgui_stats.cpp
        src/izzgui_guisystem.cpp
        src/izzgui_imguix.cpp
        src/izzgui_mainmenu.cpp
        src/izzgui_materialeditor.cpp
        src/izzgui_materialinspector.cpp
        src/izzgui_scenenavigator.cpp
        src/izzgui_shadereditor.cpp include/izzgui_shadereditor.h
        src/izzgui_windowinputlistener.cpp
        src/izzgui_docklayout.cpp
        src/izzgui_componenteditor.cpp
        include/izzgui_componenteditor.h
        include/izzgui_docklayout.h
        include/izzgui_layout.h

        # VWR - need to be extracted to a lib independent of GUI
        include/izzgui_iwindowextension.h
        include/izzgui_window.h
        include/izzgui_windowinputlistener.h
        src/izzgui_window.cpp

        # IMGUI
        src/imgui_impl_glfw.h
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.h
        src/imgui_impl_opengl3.cpp include/izzgui_mainmenu.h src/izzgui_mainmenu.cpp)

target_include_directories(izz-gui PRIVATE src PUBLIC include)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

target_include_directories(izz-gui PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty")
target_link_libraries(izz-gui
        PRIVATE
        izzy
        ecs
        izzgl
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        imgui::imgui)