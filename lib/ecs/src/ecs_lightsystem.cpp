#include <ecs_lightsystem.h>
#include <ecs_renderable.h>
#include <ecs_transform.h>
#include <spdlog/spdlog.h>
#include <ecs_light.h>
using namespace lsw;
using namespace lsw::ecs;

LightSystem::LightSystem(entt::registry& registry)
  : m_registry{registry} {}

void LightSystem::initLightingUbo(Renderable& renderable, const geo::Material& material) {
  auto uboStructName = material.lighting.ubo_struct_name;

  // check if a light ubo struct name is defined.
  if (uboStructName.empty()) {
    return;
  }

  renderable.uboLightingIndex = glGetUniformBlockIndex(renderable.program, uboStructName.c_str());
  if (renderable.uboLightingIndex == GL_INVALID_INDEX) {
    // we don't expect this. The material has defined a light struct name, but not found. Error.
    spdlog::error("Material {}: Lighting disabled, cannot find ubo block index with name '{}'", material.name, uboStructName);
    return;
  }

  renderable.isLightingSupported = true;

  // now of all the supported light systems, find the one the shader needs and store the size and address of it.
  if (uboStructName == UniformLighting::PARAM_NAME) {
    renderable.pUboLightStruct = &m_oldModel;
    renderable.pUboLightStructSize = sizeof(UniformLighting);
  } else if (uboStructName == ForwardLighting::PARAM_NAME) {
    renderable.pUboLightStruct = &m_forwardLighting;
    renderable.pUboLightStructSize = sizeof(ForwardLighting);
  } else {
    throw std::runtime_error(
        fmt::format("Material {} makes use of unsupported uniform light structure '{}'", material.name, uboStructName));
  }

  glGenBuffers(1, &renderable.uboLightingId);
  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
  GLint blockIndex = glGetUniformBlockIndex(renderable.program, uboStructName.c_str());

  glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &renderable.uboLightingBinding);

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);
  glBufferData(GL_UNIFORM_BUFFER, renderable.pUboLightStructSize, nullptr, GL_DYNAMIC_DRAW);
}

void LightSystem::updateLightProperties() {
  //
  auto pointLights = m_registry.view<Transform, PointLight>();
  auto dirLights = m_registry.view<Transform, DirectionalLight>();
  auto ambientLights = m_registry.view<AmbientLight>();
  auto numLights = pointLights.size_hint() + dirLights.size_hint() + ambientLights.size();
  if (numLights == 0) {
    std::cout << "WARNING: No active lights in scene." << std::endl;
  }
  //  if (numLights > 4) {
  //    throw std::runtime_error("Too many lights in the scene. Max supported is 4");
  //  }

  int numberOfPointLights = std::clamp(static_cast<unsigned int>(pointLights.size_hint()), 0U, 4U);
  int numberOfSpotLights = 0U;
  int numberOfDirectionalLights = std::clamp(static_cast<unsigned int>(dirLights.size_hint()), 0U, 1U);
  int numberOfAmbientLights = std::clamp(static_cast<unsigned int>(ambientLights.size()), 0U, 1U);

  m_oldModel.numberLights = numberOfDirectionalLights;
  m_forwardLighting.numberOfLights =
      glm::ivec4(numberOfDirectionalLights, numberOfAmbientLights, numberOfPointLights, numberOfSpotLights);

  m_oldModel.numberLights = 1U;

  // -- Directional lights ------------------------------
  auto i = 0U;
  for (auto e : dirLights) {
    if (i > numberOfDirectionalLights) {
      break;
    }
    const auto& light = dirLights.get<DirectionalLight>(e);
    const auto& transform = dirLights.get<Transform>(e);

    m_oldModel.diffuseColors[0] = glm::vec4(light.color, 0.0F);
    m_oldModel.intensities[0] = light.intensity;
    m_oldModel.positions[0] = transform.worldTransform * glm::vec4(1.0);

    m_forwardLighting.directionalLight.direction = transform.worldTransform * glm::vec4(1.0);
    std::cout << "Direction: " << m_forwardLighting.directionalLight.direction[0] << " "
              << m_forwardLighting.directionalLight.direction[1] << " " << m_forwardLighting.directionalLight.direction[2]
              << std::endl;
    m_forwardLighting.directionalLight.color = glm::vec4(light.color, 0.0F);
    m_forwardLighting.directionalLight.intensity = light.intensity;

    m_oldModel.positions[0] = transform.worldTransform * glm::vec4(1.0);
    m_oldModel.attenuation[0] = 0.0;
    m_oldModel.diffuseColors[0] = glm::vec4(light.color, 1.0);
    m_oldModel.intensities[0] = light.intensity;
  }

  // -- Ambient lights ------------------------------
  i = 0U;
  for (auto e : ambientLights) {
    if (i > numberOfAmbientLights) {
      break;
    }
    auto& light = ambientLights.get<AmbientLight>(e);
    m_forwardLighting.ambientLight.color = glm::vec4(light.color, 0.0F);
    m_forwardLighting.ambientLight.intensity = light.intensity;
  }

  // -- Point lights ------------------------------
  i = 0;
  for (auto e : pointLights) {
    if (i > numberOfPointLights) {
      break;
    }
    const auto& light = pointLights.get<PointLight>(e);
    const auto& transform = pointLights.get<Transform>(e);

    auto& ubo = m_forwardLighting.pointLights[i++];
    ubo.attenuation = light.attenuation;
    ubo.color = glm::vec4(light.color, 0.0F);
    ubo.intensity = light.intensity;
    ubo.position = transform.worldTransform * glm::vec4(1.0);
  }

  // -- spotlights - not supported yet -----------------------------
  //  int i = 0;
  //  for (auto e : spotlights) {
  //    if (i > numberOfPointLights) {
  //      break;
  //    }
  //    auto ptLight = pointLights.get<PointLight>(e);
  //    auto& m = m_lighting.pointLights[i++];
  //    m.attenuation = ptLight.attenuation;
  //    m.color = ptLight.color;
  //    m.intensity = ptLight.intensity;
  //    m.position = ptLight.position;
  //  }
}