add_library(inputsystem STATIC src/io_inputsystem.cpp include/io_inputsystem.h include/io_command.h include/io_inputlistener.h include/io_keyconstants.h)

target_include_directories(inputsystem
        PUBLIC include
        PRIVATE src)

target_link_libraries(inputsystem
PUBLIC CONAN_PKG::entt CONAN_PKG::glfw)
