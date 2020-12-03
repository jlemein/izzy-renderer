
add_library(fractal STATIC
        include/fractal_tree.h
        include/fractal_treefactory.h

        src/fractal_tree.cpp
        src/fractal_treefactory.cpp
        src/fractal_drawcommands.h include/fractal_rule.h include/fractal_state.h include/fractal_stochastictree.h src/fractal_stochastictree.cpp include/fractal_penrosetiling.h)

target_include_directories(fractal
        PUBLIC include
        PRIVATE src)

target_link_libraries(fractal PUBLIC glm)