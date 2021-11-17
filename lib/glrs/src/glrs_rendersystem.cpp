//
// Created by jlemein on 07-11-20.
//
#include <glrs_rendersystem.h>

#include <cstring>
#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <ecsg_scenegraph.h>
#include <fstream>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <uniform_ubermaterial.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <glrs_lightsystem.h>
#include <ecs_wireframe.h>
#include <geo_material.h>
#include <glrs_shadersystem.h>

using namespace lsw;
using namespace lsw::glrs;

namespace {


    constexpr void *BUFFER_OFFSET(unsigned int offset) {
        uint8_t *pAddress = 0;
        return pAddress + offset;
    }

    void pushShaderProperties(const Renderable &r) {
        for (const auto&[name, uniformBlock]: r.userProperties) {
            glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
            glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockBinding, uniformBlock.bufferId);

            // is this needed?
            glUniformBlockBinding(r.program, uniformBlock.blockIndex, uniformBlock.blockBinding);

            void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
            std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
    }

    void pushModelViewProjection(const Renderable &renderable) {
        glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
        glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockBinding, renderable.uboId);

        // is this needed?
        glUniformBlockBinding(renderable.program, renderable.uboBlockIndex, renderable.uboBlockBinding);

        void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        UniformBlock b = renderable.uniformBlock;
        std::memcpy(buff_ptr, &b, sizeof(UniformBlock));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    void pushLightingData(const Renderable &renderable) {
        glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
        glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);

        // is this needed?
        glUniformBlockBinding(renderable.program, renderable.uboLightingIndex, renderable.uboLightingBinding);

        void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        std::memcpy(buff_ptr, renderable.pUboLightStruct, renderable.pUboLightStructSize);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    void initMVPUniformBlock(Renderable &renderable) {
        glUseProgram(renderable.program);  // TODO: remove line
        renderable.uboBlockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");

        if (renderable.uboBlockIndex == GL_INVALID_INDEX) {
            throw std::runtime_error(
                    fmt::format("Shader program does not contain a uniform block with name 'UniformBufferBlock' in {}",
                                renderable.material ? (*renderable.material)->vertexShader : "<no material assigned>"));
        }

        glGenBuffers(1, &renderable.uboId);

        glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
        GLint blockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
        if (blockIndex == GL_INVALID_INDEX) {
            std::cerr << "Cannot find ubo block with name 'UniformBufferBlock' in shader";
            exit(1);
        }

        glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING,
                                  &renderable.uboBlockBinding);

        glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex, renderable.uboId);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr, GL_DYNAMIC_DRAW);
    }

// void initLightingUbo(Renderable& renderable, geo::Material& material) {
//   auto lightingBlockName = !material.lighting.layout.empty() ? material.lighting.layout : UniformLighting::PARAM_NAME;
//   //  auto lightingBlockName = UniformLighting::PARAM_NAME;
//
//
//
//   renderable.uboLightingIndex = glGetUniformBlockIndex(renderable.program, lightingBlockName.c_str());
//   if (renderable.uboLightingIndex == GL_INVALID_INDEX) {
//     spdlog::debug("Lighting disabled, cannot find ubo block index with name {}", UniformLighting::PARAM_NAME);
//     renderable.isLightingSupported = false;
//   } else {
//     renderable.isLightingSupported = true;
//   }
//
//   glGenBuffers(1, &renderable.uboLightingId);
//   glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
//   GLint blockIndex = glGetUniformBlockIndex(renderable.program, UniformLighting::PARAM_NAME);
//
//   glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &renderable.uboLightingBinding);
//
//   glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);
//   glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformLighting), nullptr, GL_DYNAMIC_DRAW);
// }

}  // namespace

/**
 * @brief Sets up the render component (i.e. the handle to the render system)
 * with the assigned material properties for this entity. Every material has a
 * uniform properties attribute that gets filled based on ....
 *
 * @details
 * The material component (@see geo::Material) contains a set of attributes,
 * easily editable in the code. Eventually the attributes gets mapped to a
 * uniform property attribute.
 *
 * @param renderable The render component
 * @param properties The material properties.
 */
void RenderSystem::initShaderProperties(Renderable &renderable, const geo::Material &material) {
    glUseProgram(renderable.program);

    // allocates the MVP matrices on the GPU.
    initMVPUniformBlock(renderable);
    m_lightSystem->initLightingUbo(renderable, material);

    // every shader usually has custom attributes
    // they are processed here
    for (const auto&[name, blockData]: material.properties) {
        GLuint uboHandle;
        glGenBuffers(1, &uboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

        GLint blockIndex = glGetUniformBlockIndex(renderable.program, name.c_str());
        GLint blockBinding;
        glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);

        // is this needed?
        //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);

        glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);

        if (blockIndex == GL_INVALID_INDEX) {
            auto a = glGetUniformLocation(renderable.program, name.c_str());
            throw std::runtime_error(
                    fmt::format("Cannot find ubo block with name '{}' in shader {}", name, material.name));
        }
        glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

//    auto p = reinterpret_cast<ufm::UberMaterial*>(blockData.data);
//    std::cout << "Ubermaterial: hasDiffuse " << std::boolalpha << p->hasDiffuseTex << std::endl;
        //    std::cout << "   --- Init " << renderable.name << ">" << name << "
        //    with "
        //              << p->diffuse.r << " " << p->diffuse.g << " " <<
        //              p->diffuse.b
        //              << std::endl;

        // store block handle in renderable
        renderable.userProperties[name] = Renderable_UniformBlockInfo{uboHandle, blockIndex, blockBinding, &blockData};
    }
}

RenderSystem::RenderSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph,
                           std::shared_ptr<IMaterialSystem> materialSystem)
        : m_registry{sceneGraph->getRegistry()}, m_debugSystem(sceneGraph->getRegistry()),
          m_materialSystem(materialSystem), m_shaderSystem(std::make_shared<ShaderSystem>()),
          m_lightSystem{std::make_shared<LightSystem>(m_registry)} {


//    glGetIntegerv(GL_SHADER_BINARY_FORMATS)

}

void initCurveBuffers(Renderable &renderable, const geo::Curve &curve) {
    glGenBuffers(1, &renderable.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, curve.vertices.size() * sizeof(float), curve.vertices.data(), GL_STATIC_DRAW);

    renderable.index_buffer = 0U;

    renderable.vertexAttribCount = 1;
    renderable.vertexAttribArray[0].size = 3U;
    renderable.vertexAttribArray[0].buffer_offset = 0U;
    renderable.drawElementCount = curve.vertices.size() / 3;
    renderable.primitiveType = GL_LINES;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void initMeshBuffers(Renderable &renderable, const geo::Mesh &mesh) {
    GLuint vertexSize = mesh.vertices.size() * sizeof(float);
    GLuint normalSize = mesh.normals.size() * sizeof(float);
    GLuint uvSize = mesh.uvs.size() * sizeof(float);
    GLuint tangentSize = mesh.tangents.size() * sizeof(float);
    GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
    GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;

    glGenBuffers(1, &renderable.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr,
                 GL_STATIC_DRAW);  // allocate buffer data only
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize,
                    mesh.vertices.data());  // fill partial data - vertices
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize,
                    mesh.normals.data());  // fill partial data - normals
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize,
                    mesh.uvs.data());  // fill partial data - normals

    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize, tangentSize,
                    mesh.tangents.data());  // fill partial data - normals

    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize + tangentSize, bitangentSize,
                    mesh.bitangents.data());  // fill partial data - normals

    glGenBuffers(1, &renderable.index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    renderable.vertexAttribCount = 5;
    // position data
    renderable.vertexAttribArray[0].size = 3U;
    renderable.vertexAttribArray[0].buffer_offset = 0U;
    // normal data
    renderable.vertexAttribArray[1].size = 3U;
    renderable.vertexAttribArray[1].buffer_offset = vertexSize;
    // uv data
    renderable.vertexAttribArray[2].size = 2U;
    renderable.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
    // tangent data
    renderable.vertexAttribArray[3].size = 3U;
    renderable.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
    // bitangent data
    renderable.vertexAttribArray[4].size = 3U;
    renderable.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;

    renderable.drawElementCount = mesh.indices.size();
    renderable.primitiveType = GL_TRIANGLES;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize + tangentSize));
}
//
//RenderSystem::ShaderProgram RenderSystem::compileShader(const geo::Material& material, const Renderable& renderable) {
////  auto vertexShaderBuffer = readFile(material.vertexShader);
////  auto fragmentShaderBuffer = readFile(material.fragmentShader);
//
//  auto vertexShaderBuffer = readFile(material.vertexShader);
//  auto fragmentShaderBuffer = readFile(material.fragmentShader);
//
//  int vsCompiled = 0, fsCompiled = 0;
//  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//  glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertexShaderBuffer.data(), vertexShaderBuffer.size());
//  glSpecializeShaderARB(vertex_shader, "main", 0, 0, 0);
//  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);
//
//  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//  glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fragmentShaderBuffer.data(),
//                 fragmentShaderBuffer.size());
//  glSpecializeShaderARB(fragment_shader, "main", 0, 0, 0);
//  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsCompiled);
//
//  if (!fsCompiled) {
//    spdlog::log(spdlog::level::err, "Failed to compile fragment shader");
//    getShaderLog(fragment_shader);
//  }
//
//  if (!vsCompiled) {
//    spdlog::log(spdlog::level::err, "Failed to compile vertex shader");
//    getShaderLog(vertex_shader);
//  }
//
//  if (!fsCompiled || !vsCompiled) {
//    exit(1);
//  }
//
//  auto program = glCreateProgram();
//  glAttachShader(program, vertex_shader);
//  glAttachShader(program, fragment_shader);
//  glLinkProgram(program);
//  int linked = 0;
//  glGetProgramiv(program, GL_LINK_STATUS, &linked);
//
//  if (linked == GL_FALSE) {
//    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}", material.vertexShader, material.fragmentShader);
//    printLinkingLog(program);
//  }
//
//  return static_cast<RenderSystem::ShaderProgram>(program);
//}

void RenderSystem::init() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    // convert material descriptions to openGL specific material data.
    m_materialSystem->synchronizeTextures(*this);

    auto numMaterials = m_registry.view<geo::Material>().size();
    auto numLights = m_lightSystem->getActiveLightCount();

    // small summary
    spdlog::info(
            "Render system initialized | "
            "Number of material in use: {} | "
            "Number of active lights: {}",
            numMaterials, numLights);

    // Initialization of the rendersystem encompasses the following steps.
    // Take into account the vocabulary.
    // * init: sets up the buffers, do precomputations and store parameters in
    // renderable component.
    // * update: reflect or synchronize the state of the system in the renderable
    // component.
    // * push: Reflect the changes to the GPU.
    //
    // * Synchronize the model view projection matrices into the renderable
    // component.
    // * Update the renderable component: allocate buffers and load mesh data in
    // GPU
    // * Push the remaining uniform parameter values to the shader.

    // handling curves
    auto curveView = m_registry.view<geo::Curve, Renderable>();
    for (auto entity: curveView) {
        auto &curve = m_registry.get<geo::Curve>(entity);
        auto &renderable = m_registry.get<Renderable>(entity);
        auto &material = m_registry.get<geo::Material>(entity);

        try {
            initCurveBuffers(renderable, curve);

            if (material.isBinaryShader) {
                renderable.program = m_shaderSystem->compileSpirvShader(material.vertexShader, material.fragmentShader);
            } else {
                renderable.program = m_shaderSystem->compileShader(material.vertexShader, material.fragmentShader);
            }
            initShaderProperties(renderable, material);
        } catch (std::exception &e) {
            auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";

            std::cerr << "Failed initializing curve '" << name << "': " << e.what();
            exit(1);
        }
    }

    // handling meshes
    auto view = m_registry.view<geo::Mesh, geo::Material, Renderable>();
    for (auto entity: view) {
        auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";

        try {
            auto &renderable = m_registry.get<Renderable>(entity);
            auto &mesh = m_registry.get<geo::Mesh>(entity);
            initMeshBuffers(renderable, mesh);

            auto &material = m_registry.get<geo::Material>(entity);
            spdlog::debug("Compiling shaders -- vs: {} fs: {}", material.vertexShader, material.fragmentShader);

            if (material.isBinaryShader) {
                renderable.program = m_shaderSystem->compileSpirvShader(material.vertexShader, material.fragmentShader);
            } else {
                renderable.program = m_shaderSystem->compileShader(material.vertexShader, material.fragmentShader);
            }

            initShaderProperties(renderable, material);

        } catch (std::exception &e) {
            auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";
            throw std::runtime_error(fmt::format("Failed initializing mesh '{}': {}", name, e.what()));
        }
    }
}

void RenderSystem::update(float time, float dt) {
    // synchronizes the transformation for the entity into the renderable
    // component.
    synchMvpMatrices();
    m_lightSystem->updateLightProperties();
    m_materialSystem->update(time, dt);

    //  m_materialSystem->update(time, dt); --> not needed I think
}

void RenderSystem::synchMvpMatrices() {
    // Updates the
    // Render system updates the model view projection matrix for each of the
    // The camera
    auto view = m_registry.view<Renderable>();
    for (auto entity: view) {
        auto &renderable = m_registry.get<Renderable>(entity);

        updateModelMatrix(entity);
        updateCamera(renderable);
    }
}
//
// void RenderSystem::updateLightProperties() {
//
//  auto view = m_registry.view<Transform, Light>();
//  m_uLightData.numberLights = view.size_hint();
//
//  if (view.size_hint() == 0) {
//    std::cout << "WARNING: No active lights in scene." << std::endl;
//  }
//  if (view.size_hint() > 4) {
//    throw std::runtime_error("Too many lights in the scene. Max supported is 4");
//  }
//
//  unsigned int i = 0U;
//  for (auto e : view) {
//    const auto& light = view.get<Light>(e);
//    const auto& transform = view.get<Transform>(e);
//    const auto& name = m_registry.get<Name>(e);
//
//    m_uLightData.diffuseColors[i] = glm::vec4(light.diffuseColor, 1.0F);
//    m_uLightData.intensities[i] = light.intensity;
//    m_uLightData.attenuation[i] = light.attenuationQuadratic;
//    m_uLightData.positions[i] = transform.worldTransform * glm::vec4(1.0);
//    ++i;
//  }
//}

void RenderSystem::updateModelMatrix(entt::entity e) {
    auto &renderable = m_registry.get<Renderable>(e);

    // if the transformation matrix exists, apply it, otherwise take identity
    // matrix.
    // TODO: enforce that all entities do have a transform.
    auto transform = m_registry.try_get<ecs::Transform>(e);

    renderable.uniformBlock.model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);
}

void RenderSystem::setActiveCamera(entt::entity cameraEntity) {
    if (!m_registry.all_of<ecs::Camera>(cameraEntity)) {
        throw std::runtime_error("Only entities with a Camera component can be set as active camera");
    }
    m_activeCamera = cameraEntity;

    // determine frame buffers
    if (m_registry.all_of<ecs::PosteffectCollection>(m_activeCamera)) {
        GLuint framebufferName = 0;
        glGenFramebuffers(1, &framebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);

        // texture we are going to render to
        GLuint renderedTexture;
        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // The depth buffer
        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    }
}

void RenderSystem::updateCamera(Renderable &renderable) {
    if (m_activeCamera == entt::null) {
        throw std::runtime_error("No active camera in scene");
    }

    auto &transform = m_registry.get<ecs::Transform>(m_activeCamera);
    auto &activeCamera = m_registry.get<ecs::Camera>(m_activeCamera);

    renderable.uniformBlock.view = glm::inverse(transform.worldTransform);
    renderable.uniformBlock.proj =
            glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
    renderable.uniformBlock.viewPos = glm::vec3(transform.worldTransform[3]);
}

void RenderSystem::addSubsystem(std::shared_ptr<IRenderSubsystem> system) {
    m_renderSubsystems.emplace_back(system);
}

void RenderSystem::removeSubsystem(std::shared_ptr<IRenderSubsystem> system) {
    // todo
}

void RenderSystem::render() {

    // 1. Select buffer to render into
    auto view = m_registry.view<const Renderable>();

    for (auto entity: view) {
        const auto &renderable = view.get<const Renderable>(entity);
        const auto &name = m_registry.get<ecs::Name>(entity);

        // activate shader program
        glUseProgram(renderable.program);

        // prepare the materials
        activateTextures(entity);

        // TODO: disable in release
        if (renderable.isWireframe || m_registry.any_of<ecs::Wireframe>(entity)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // bind the vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);

        for (unsigned int i = 0U; i < renderable.vertexAttribCount; ++i) {
            const VertexAttribArray &attrib = renderable.vertexAttribArray[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
        }

        //    glBindVertexArray(renderable.vertex_array_object);

        // assign textures
        for (auto &subsystem: m_renderSubsystems) {
            subsystem->onRender(m_registry, entity);
        }

        // TODO: check if shader is dirty
        //  reason: if we push properties every frame (Except for MVP), we might
        //  unnecessary spend time doing that while we can immediately just render.
        pushShaderProperties(renderable);
        pushModelViewProjection(renderable);

        // TODO this one needs to change per glUseProgram, which is the case right
        //  now. In future we might optimize changing of glUseProgram in that
        //  case, this function should be called once per set of glUseProgram.
        if (renderable.isLightingSupported) {
            pushLightingData(renderable);
        }

        if (renderable.primitiveType == GL_TRIANGLES) {
            glDrawElements(renderable.primitiveType, renderable.drawElementCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(renderable.primitiveType, 0, renderable.drawElementCount);
        }

        // handle debug
        checkError(entity);
    }
}

void RenderSystem::checkError(entt::entity e) {
    // get error message
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        auto name = m_registry.all_of<ecs::Name>(e) ? m_registry.get<ecs::Name>(e).name : std::string{"Unnamed"};
        std::cerr << " Render error occurred for " << name << ": " << err << std::endl;
    }
}

/**!
 * @brief Creates a texture buffer and sends the data over to the GPU. This
 * method therefore converts a texture resource to GPU ready representation.
 * @param renderable      [in/out] Reference to renderable
 * @param geoTexture
 * @param name
 *
 * TODO: think about passing a scene graph entity instead
 * @return
 */
void RenderSystem::attachTexture(Renderable &renderable, const geo::Texture &geoTexture, const std::string &name) {
    ecs::Texture texture{.name = name};
    glGenTextures(1, &texture.glTextureId);
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (!geoTexture.data.empty()) {
        GLint texChannel = geoTexture.channels == 3 ? GL_RGB : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, geoTexture.width, geoTexture.height, 0, texChannel, GL_UNSIGNED_BYTE,
                     geoTexture.data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        spdlog::error("Failed to create texture buffer for texture '{}'. Data is empty.", geoTexture.path);
        exit(1);
    }

    renderable.textures.emplace_back(texture);
    spdlog::log(spdlog::level::debug, "Loaded GL texture for texture '{}'", name);
}

void RenderSystem::activateTextures(entt::entity e) {
    //  for (auto entity : view) {
    //  auto& registry = m_sceneGraph->getRegistry();

    if (!m_registry.all_of<Renderable, geo::Material>(e)) {
        return;
    }

    const auto &renderable = m_registry.get<Renderable>(e);
    //  const auto& shader = m_registry.get<Shader>(e);

    for (int t = 0; t < renderable.textures.size(); ++t) {
        auto &texture = renderable.textures[t];

        glActiveTexture(GL_TEXTURE0 + t);
        glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

        GLint s_diffuseMap = glGetUniformLocation(renderable.program, texture.name.c_str());
        //    if (s_diffuseMap == -1) {
        //        //std::cout << "Cannot find diffuse texture" << std::endl;
        //        //exit(1);
        //    } else {
        glUniform1i(s_diffuseMap, t);
    }

    //    }
    //    glBindSampler(0, 0);
}