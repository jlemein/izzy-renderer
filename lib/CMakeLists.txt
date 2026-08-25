# A lot of libraries here
# The major libraries will be the first subset on the next following lines.
# Refactoring in progress.
add_subdirectory(izzgui)   # lsw::gui - Gui implementations using ImGui
add_subdirectory(izzgl)  # OpenGL based Render System

# the following libraries will be refactored
add_subdirectory(workspace)
add_subdirectory(fractal)
add_subdirectory(fractal_generator)
add_subdirectory(ecs_inputsystem)
add_subdirectory(ecs)
# Disabled: ecsw_model.h includes ecsw_material.h, which does not exist in this tree, so the
# library has never compiled. Nothing links ecsw. Kept out of the build until it is revived.
#add_subdirectory(ecsw)
add_subdirectory(geo)
