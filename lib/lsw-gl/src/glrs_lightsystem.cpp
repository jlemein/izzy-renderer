#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_transform.h>
#include <geo_materialutil.h>
#include <geo_mesh.h>
#include <gl_renderable.h>
#include <glrs_lightsystem.h>
#include <spdlog/spdlog.h>
#include "izzgl_material.h"
#include "izzgl_materialsystem.h"
using namespace izz;
using namespace izz::gl;

namespace {
void updatePointLightVisualization(Material& material, lsw::ecs::PointLight light) {
//  material.setUniformVec4("color", glm::vec4(light.color, 0.0));
//  material.setUniformFloat("radius", light.radius);
  material.setUniformFloat("intensity", light.intensity);
}
}

LightSystem::LightSystem(entt::registry& registry, std::shared_ptr<MaterialSystem> materialSystem)
  : m_registry{registry}, m_materialSystem{materialSystem} {}

int LightSystem::getActiveLightCount() const {
  auto pointLights = m_registry.view<lsw::ecs::Transform, lsw::ecs::PointLight>();
  auto dirLights = m_registry.view<lsw::ecs::Transform, lsw::ecs::DirectionalLight>();
  auto ambientLights = m_registry.view<lsw::ecs::AmbientLight>();

  return pointLights.size_hint() + dirLights.size_hint() + ambientLights.size();
}

void LightSystem::setDefaultPointLightMaterial(int materialId) {
  m_lightMaterial = materialId;
}

void LightSystem::initialize() {
  spdlog::debug("Initializing light system");
  for (auto&& [e, light, mesh] : m_registry.view<lsw::ecs::PointLight, lsw::geo::Mesh>().each()) {
    if (!m_registry.all_of<izz::gl::Material>(e)) {
      if (m_lightMaterial == -1) {
        auto name = m_registry.get<lsw::ecs::Name>(e).name;
        spdlog::error("Cannot add a material for point light '{}'. No light material set", name);
      }
      else {
//        m_registry.emplace<Renderable>(e);

//        auto& material = m_registry.emplace<izz::gl::Material>(e, izz::gl::MaterialUtil::CloneMaterial(*m_lightMaterial));
        updatePointLightVisualization(m_materialSystem->getMaterialById(m_lightMaterial), light);
      }
    }
  }
}

//void LightSystem::initLightingUbo(RenderState& renderable, const izz::gl::Material& material) {
//  auto uboStructName = material.lighting.ubo_struct_name;
//
//  // check if a light ubo struct name is defined.
//  if (uboStructName.empty()) {
//    return;
//  }
//
//  renderable.uboLightingIndex = glGetUniformBlockIndex(renderable.program, uboStructName.c_str());
//  if (renderable.uboLightingIndex == GL_INVALID_INDEX) {
//    // we don't expect this. The material has defined a light struct name, but not found. Error.
//    spdlog::error("Material {}: Lighting disabled, cannot find ubo block index with name '{}'", material.name, uboStructName);
//    return;
//  }
//
//  renderable.isLightingSupported = true;
//
//  // now of all the supported light systems, find the one the shader needs and store the size and address of it.
//  if (uboStructName == ForwardLighting::PARAM_NAME) {
//    renderable.pUboLightStruct = &m_forwardLighting;
//    renderable.pUboLightStructSize = sizeof(ForwardLighting);
//  } else {
//    throw std::runtime_error(fmt::format("Material {} makes use of unsupported uniform light structure '{}'", material.name, uboStructName));
//  }
//
//  glGenBuffers(1, &renderable.uboLightingId);
//  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
//  GLint blockIndex = glGetUniformBlockIndex(renderable.program, uboStructName.c_str());
//
//  glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &renderable.uboLightingBinding);
//
//  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);
//  glBufferData(GL_UNIFORM_BUFFER, renderable.pUboLightStructSize, nullptr, GL_DYNAMIC_DRAW);
//}

void LightSystem::updateLightProperties() {
  //
  auto pointLights = m_registry.view<lsw::ecs::Transform, lsw::ecs::PointLight>();
  auto dirLights = m_registry.view<lsw::ecs::Transform, lsw::ecs::DirectionalLight>();
  auto ambientLights = m_registry.view<lsw::ecs::AmbientLight>();
  auto numLights = pointLights.size_hint() + dirLights.size_hint() + ambientLights.size();

  int numberOfPointLights = std::clamp(static_cast<unsigned int>(pointLights.size_hint()), 0U, 4U);
  int numberOfSpotLights = 0U;
  int numberOfDirectionalLights = std::clamp(static_cast<unsigned int>(dirLights.size_hint()), 0U, 1U);
  int numberOfAmbientLights = std::clamp(static_cast<unsigned int>(ambientLights.size()), 0U, 1U);

  m_forwardLighting.numberOfLights = glm::ivec4(numberOfDirectionalLights, numberOfAmbientLights, numberOfPointLights, numberOfSpotLights);

  // -- Directional lights ------------------------------
  auto i = 0U;
  for (auto e : dirLights) {
    if (i++ > numberOfDirectionalLights) {
      break;
    }
    const auto& light = dirLights.get<lsw::ecs::DirectionalLight>(e);
    const auto& transform = dirLights.get<lsw::ecs::Transform>(e);

    m_forwardLighting.directionalLight.direction = transform.worldTransform[3];
    m_forwardLighting.directionalLight.color = glm::vec4(light.color, 0.0F);
    m_forwardLighting.directionalLight.intensity = light.intensity;
  }

  // -- Ambient lights ------------------------------
  i = 0U;
  for (auto e : ambientLights) {
    if (i++ > numberOfAmbientLights) {
      break;
    }
    auto& light = ambientLights.get<lsw::ecs::AmbientLight>(e);
    m_forwardLighting.ambientLight.color = glm::vec4(light.color, 0.0F);
    m_forwardLighting.ambientLight.intensity = light.intensity;
  }

  // -- Point lights ------------------------------
  i = 0;
  for (auto e : pointLights) {
    if (i > numberOfPointLights) {
      break;
    }
    const auto& light = pointLights.get<lsw::ecs::PointLight>(e);
    const auto& transform = pointLights.get<lsw::ecs::Transform>(e);

    auto& ubo = m_forwardLighting.pointLights[i++];
    ubo.linearAttenuation = light.linearAttenuation;
    ubo.quadraticAttenuation = light.quadraticAttenuation;
    ubo.color = glm::vec4(light.color, 0.0F);
    ubo.intensity = light.intensity;
    ubo.position = transform.worldTransform[3];

    // if the point light has a point light visualization
    if (m_registry.all_of<izz::gl::Material>(e)) {
      auto& material = m_registry.get<izz::gl::Material>(e);
      updatePointLightVisualization(material, light);
    }
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