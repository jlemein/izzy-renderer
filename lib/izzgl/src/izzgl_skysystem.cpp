//
// Created by jeffrey on 3-1-23.
//
#include <geo_mesh.h>
#include <glrs_lightsystem.h>
#include <izzgl_gammacorrectionpe.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgl_skysystem.h>
#include <izzgl_texturesystem.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include "geo_primitivefactory.h"
#include "izz_skybox.h"
#include "izzgl_error.h"
#include "izzgl_shadersystem.h"
using namespace izz::gl;

SkySystem::SkySystem(entt::registry& registry, std::shared_ptr<izz::gl::MaterialSystem> materialSystem, std::shared_ptr<TextureSystem> textureSystem,
                     std::shared_ptr<izz::gl::MeshSystem> meshSystem)
  : m_registry{registry}
  , m_materialSystem{materialSystem}
  , m_textureSystem{textureSystem}
  , m_meshSystem{meshSystem} {

  m_registry.on_construct<izz::Skybox>().connect<&SkySystem::onSkyboxModified>(this);
}

void SkySystem::onSkyboxModified(entt::registry& registry, entt::entity e) {
  // if deferred entity has no renderable component, then it will be added.
  // we need the renderable for the common rendering functionalities (such as mvp updates).
  auto skybox = m_registry.get<izz::Skybox>(e);
  auto& skyboxMaterial = m_materialSystem->getMaterialById(skybox.material);

  if (skyboxMaterial.hasTexture(TextureTag::ENVIRONMENT_MAP)) {
    if (skybox.isEnabled) {
      TextureId id = skyboxMaterial.getTexture(TextureTag::ENVIRONMENT_MAP);
      Texture* environmentMap = m_textureSystem->getTextureById(id);

      for (auto& [id, material] : m_materialSystem->getCreatedMaterials()) {
        if (material.id != skyboxMaterial.id && material.hasTexture(TextureTag::ENVIRONMENT_MAP)) {
          material.setTexture(TextureTag::ENVIRONMENT_MAP, environmentMap);
        }
        if (material.hasTexture(TextureTag::IRRADIANCE_MAP)) {
          material.setTexture(TextureTag::IRRADIANCE_MAP, environmentMap);
        }
      }
    } else {
      //      Texture* nullTexture = m_resourceManager->getTextureSystem()->getNullTexture();
      spdlog::warn("Cannot unset the environment map texture");
    }
  }
}

void SkySystem::init(int width, int height) {
  m_width = width;
  m_height = height;

  auto box = izz::geo::PrimitiveFactory::MakeBox("UnitCube");
  izz::geo::Mesh mesh;
  mesh.vertices.reserve(sizeof(Skybox::Vertices) / sizeof(float));
  float* begin = Skybox::Vertices;
  float* end = begin + sizeof(Skybox::Vertices) / sizeof(float);
  mesh.vertices = std::vector<float>(begin, end);
  //  m_unitCubeVertexBufferId = m_meshSystem->createVertexBuffer(mesh).id;
  m_unitCubeVertexBufferId = m_meshSystem->createVertexBuffer(box).id;

  // explicitly call this the first time of initialization.
  auto skyboxes = m_registry.view<izz::Skybox>();
  if (skyboxes.size() > 0) {
    onSkyboxModified(m_registry, skyboxes[0]);
  }
}

void SkySystem::update(float dt, float time) {
//  auto view = m_registry.view<izz::Skybox>();
//  if (view.size() != 0) {
//    auto e = view[0];
//    auto& skybox = m_registry.get<izz::Skybox>(e);
//
}

void SkySystem::render() {
  auto skyboxesView = m_registry.view<izz::Skybox>();
  if (skyboxesView.size() != 0) {
    auto e = skyboxesView[0];
    auto& skybox = m_registry.get<izz::Skybox>(e);

    if (!skybox.isEnabled) {
      return;
    }

    auto& material = m_materialSystem->getMaterialById(skybox.material);
    auto& buffer = m_meshSystem->getMeshBuffer(m_unitCubeVertexBufferId);

    m_materialSystem->updateUniformsForEntity(e, material);

    material.useProgram();
    material.pushUniforms();
    material.useTextures();
    //    m_materialSystem->bindMaterial(material);
    m_meshSystem->bindBuffer(buffer);

    //    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    //    glDrawArrays(buffer.primitiveType, 0, buffer.drawElementCount*3);
    glDrawElements(buffer.primitiveType, buffer.drawElementCount, GL_UNSIGNED_INT, 0);
    //    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    //    glDepthFunc(GL_LESS);
  }
}

void SkySystem::setEnvironmentMap(Texture* environmentMap, Texture* convolutedMap) {
  spdlog::error("Recomputing the convoluted map");
}

void SkySystem::resize(int width, int height) {
  m_width = width;
  m_height = height;
}