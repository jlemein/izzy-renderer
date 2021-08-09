
message("Project is: ${PROJECT_SOURCE_DIR}")

add_library(simpleviewer STATIC src/viewer.cpp
        include/viewer_windowinputlistener.h src/viewer_windowinputlistener.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(EnTT REQUIRED)

target_include_directories(simpleviewer PUBLIC include
        PRIVATE src ${PROJECT_SOURCE_DIR}/thirdparty/stb)
target_link_libraries(simpleviewer PUBLIC
        EnTT::EnTT
        glfw::glfw
        OpenGL::GL
        GLEW::GLEW
        glm::glm
        inputsystem ecs_system
        )
