add_library(inputsystem STATIC src/io_inputsystem.cpp include/io_inputsystem.h include/io_command.h include/io_inputlistener.h include/io_keyconstants.h)

target_include_directories(inputsystem
        PUBLIC include
        PRIVATE src)

find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(inputsystem
PUBLIC EnTT::EnTT glfw)
