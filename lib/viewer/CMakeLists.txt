
message("Project is: ${PROJECT_SOURCE_DIR}")

add_library(simpleviewer STATIC src/viewer.cpp
        include/viewer_windowinputlistener.h src/viewer_windowinputlistener.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
target_include_directories(simpleviewer PUBLIC include
        PRIVATE src ${PROJECT_SOURCE_DIR}/thirdparty/stb)
target_link_libraries(simpleviewer PUBLIC EnTT glfw OpenGL::GL GLEW::GLEW glm inputsystem ecs_system
        )