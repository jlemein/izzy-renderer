# A lot of libraries here
# The major libraries will be the first subset on the next following lines.
# Refactoring in progress.
add_subdirectory(core)  # lsw::core     Core vocabulary, systems and services of izzy.
add_subdirectory(gui)   # lsw::gui - Gui implementations using ImGui

# the following libraries will be refactored
add_subdirectory(workspace)
add_subdirectory(fractal)
add_subdirectory(fractal_generator)
add_subdirectory(ecs_inputsystem)
add_subdirectory(ecs)
add_subdirectory(ecsg)
add_subdirectory(ecsw)
add_subdirectory(geo)
add_subdirectory(georm)
add_subdirectory(glrs)  # OpenGL based Render System