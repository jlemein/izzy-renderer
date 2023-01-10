add_library(izzgl STATIC
        include/gl_common.h
        include/izzgl_deferredrenderer.h
        include/izzgl_forwardrenderer.h
        include/gl_hdrframebuffer.h
        include/gl_iframebuffer.h
        include/gl_multipassframebuffer.h
        include/gl_renderable.h
        include/gl_renderutils.h
        include/glrs_lightsystem.h
        include/izzgl_deferredrenderablefactory.h
        include/izzgl_depthpeeling.h
        include/izzgl_exrloader.h
        include/izzgl_material.h
        include/izzgl_materialreader.h
        include/izzgl_materialsystem.h
        include/izzgl_meshbuffer.h
        include/izzgl_framebuffer.h
        src/izzgl_framebuffer.cpp
        src/izzgl_postprocessor.cpp
        include/izzgl_postprocessor.h
        include/izzgl_meshsystem.h
        include/izzgl_rendersystem.h
        include/izzgl_sceneloader.h
        include/izzgl_shadersource.h
        include/izzgl_stbtextureloader.h
        include/izzgl_textureloader.h
        include/izzgl_texturesystem.h
        include/uniform_albedospecularity.h
        include/uniform_blinnphong.h
        include/uniform_blinnphongsimple.h
        include/uniform_constant.h
        include/uniform_deferredlighting.h
        include/uniform_lambert.h
        include/uniform_mvp.h
        include/uniform_parallax.h
        include/uniform_ubermaterial.h
        include/uniform_uniformblockmanager.h
        src/izzgl_common.cpp
        src/izzgl_deferredrenderer.cpp
        src/izzgl_forwardrenderer.cpp
        src/gl_hdrframebuffer.cpp
        src/gl_multipassframebuffer.cpp
        src/gl_renderutils.cpp
        src/izzgl_gpu.cpp
        include/izzgl_gpu.h
        src/glrs_lightsystem.cpp
        src/glrs_shadersystem.t.cpp
        src/izzgl_deferredrenderablefactory.cpp
        src/izzgl_depthpeeling.cpp
        src/izzgl_entityfactory.cpp
        src/izzgl_exrloader.cpp
        src/izzgl_material.cpp
        src/izzgl_materialreader.cpp
        src/izzgl_materialsystem.cpp
        src/izzgl_meshsystem.cpp
        src/izzgl_rendersystem.cpp
        src/izzgl_sceneloader.cpp
        src/izzgl_shadersource.cpp
        src/izzgl_shadersystem.cpp
        include/izzgl_shadersystem.h
        src/izzgl_stbtextureloader.cpp
        src/izzgl_texturesystem.cpp
        src/uniform_ubermaterial.cpp
        src/izzgl_gammacorrection.cpp
        include/izzgl_gammacorrectionpe.h
        src/izzgl_skysystem.cpp
        src/izzgl_framebufferbuilder.cpp
        include/izzgl_framebufferbuilder.h
        include/izzgl_skysystem.h
#        include/ecs_debugshapefactory.h
#        include/izzgl_effectsystem.h
#        src/ecs_debugshapefactory.cpp
#        src/ecs_lightsystem.t.cpp
#        src/izzgl_effectsystem.cpp
        )
#test/node.t.cpp

find_package(EnTT REQUIRED)
find_package(spdlog REQUIRED)
target_include_directories(izzgl PUBLIC include PRIVATE src)
target_link_libraries(izzgl PUBLIC izzy workspace ecs geo EnTT::EnTT spdlog::spdlog)


find_package(GTest REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
add_executable(izzgl-test
#        test/deferredrendersystem.t.cpp
#        test/izzgl_rendercapabilities.t.cpp
#        test/izzgl_material.t.cpp
#        test/izzgl_shadercompiler.t.cpp
#        test/izzgl_shadercontext.t.cpp
        test/izzgl_exrloader.t.cpp)
target_link_libraries(izzgl-test PUBLIC gtest::gtest izzgl geo OpenGL::GL GLEW::GLEW)