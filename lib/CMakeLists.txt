# A lot of libraries here
# The major libraries will be the first subset on the next following lines.
# Refactoring in progress.
add_subdirectory(lsw-gui)   # lsw::gui - Gui implementations using ImGui
add_subdirectory(lsw-gl)  # OpenGL based Render System

# the following libraries will be refactored
add_subdirectory(workspace)
add_subdirectory(fractal)
add_subdirectory(fractal_generator)
add_subdirectory(ecs_inputsystem)
add_subdirectory(ecs)
add_subdirectory(ecsw)
add_subdirectory(geo)
