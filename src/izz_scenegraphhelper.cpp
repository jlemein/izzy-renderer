#include "izz_scenegraphhelper.h"
#include "izz_scenegraphentity.h"

#include "ecs_camera.h"
#include "ecs_light.h"
#include "ecs_name.h"
#include "ecs_relationship.h"
#include "ecs_texture.h"
#include "ecs_transform.h"

#include "gl_renderable.h"

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include "geo_camera.h"
#include "geo_curve.h"
#include "geo_light.h"
#include "geo_mesh.h"
#include "geo_meshinstance.h"
#include "geo_meshtransform.h"
#include "geo_scene.h"
#include "gl_deferredrenderer.h"
#include "izzgl_material.h"

using namespace izz;
using namespace lsw;
using namespace lsw::ecs;

SceneGraphHelper::SceneGraphHelper(entt::registry& registry, std::unique_ptr<RenderableComponentFactory> renderableComponentFactory)
  : m_registry{registry}
  , m_renderableComponentFactory{std::move(renderableComponentFactory)} {}

void SceneGraphHelper::setDefaultMaterial(std::shared_ptr<lsw::geo::Material> material) {
  m_defaultMaterial = material;
}

SceneGraphEntity SceneGraphHelper::addGeometry(lsw::geo::Mesh mesh, int materialId) {
  auto e = makeMoveableEntity(mesh.name);
  e.add<lsw::geo::Mesh>(mesh);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);

  // TODO: make sure we store a shared ptr instead of a copy.
  //  shared materials offer option to share materials.
//  e.add<lsw::geo::Material>(*mat);

  return e;
}

//SceneGraphEntity SceneGraphHelper::addGeometry(lsw::geo::Mesh mesh, geo::cEffect effect) {
//  auto e = makeMoveableEntity(mesh.name);
//  e.add<lsw::geo::Mesh>(mesh);
//  // TODO: make sure we store a shared ptr instead of a copy.
//  //  shared materials offer option to share materials.
//  e.add<geo::cEffect>(effect);
//  m_renderableComponentFactory->addRenderableComponent(m_registry, e, );
//
//  return e;
//}

// SceneGraphEntity SceneGraphHelper::addGeometry(lsw::geo::Mesh&& mesh, lsw::geo::Material&& material) {
//   auto e = makeEntity(mesh.name);
//   e.add<RENDERABLE_COMPONENT>();
//   e.add<lsw::geo::Mesh>(std::forward<lsw::geo::Mesh>(mesh));
//   e.add<lsw::geo::Material>(std::forward<lsw::geo::Material>(material));
//
//   return e;
// }

SceneGraphEntity SceneGraphHelper::makeEntity(std::string name) {
  auto e{m_registry.create()};
  m_registry.emplace<Name>(e, name);
  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity SceneGraphHelper::makeMoveableEntity(std::string name) {
  auto e{m_registry.create()};
  m_registry.emplace<Name>(e, name);
  m_registry.emplace<Transform>(e);
  m_registry.emplace<ecs::Relationship>(e);
  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity SceneGraphHelper::makeCamera(std::string name, float zDistance, float fovx, float aspect, float zNear, float zFar) {
  auto cameraEntity = makeMoveableEntity(std::move(name));
  ecs::Camera camera{.fovx = fovx, .aspect = aspect, .zNear = zNear, .zFar = zFar};
  cameraEntity.add<ecs::Camera>(std::move(camera));
  cameraEntity.get<ecs::Transform>().localTransform[3] = glm::vec4(0.0F, 0.0F, zDistance, 1.0F);

  return cameraEntity;
}

SceneGraphEntity SceneGraphHelper::makeCamera(const lsw::geo::Camera& geoCamera) {
  auto cameraEntity = makeMoveableEntity(geoCamera.name);
  ecs::Camera camera{.fovx = geoCamera.fovx, .aspect = geoCamera.aspect, .zNear = geoCamera.near, .zFar = geoCamera.far};
  cameraEntity.add<ecs::Camera>(std::move(camera));

  auto& transform = cameraEntity.get<Transform>();
  transform.localTransform = glm::lookAt(geoCamera.position, geoCamera.lookAt, geoCamera.up);

  return cameraEntity;
}

SceneGraphEntity SceneGraphHelper::makeLight(const lsw::geo::Light& light) {
  auto lightEntity = makeMoveableEntity(light.name);

  auto& transform = lightEntity.get<Transform>();
  transform.localTransform[3] = glm::vec4(light.position, 1.0F);

  if (light.type == lsw::geo::Light::Type::DIRECTIONAL_LIGHT) {
    transform.localTransform[3].w = 0.0F;
    lightEntity.add<ecs::DirectionalLight>({light.intensity, light.diffuseColor});
  } else if (light.type == lsw::geo::Light::Type::AMBIENT_LIGHT) {
    lightEntity.add<ecs::AmbientLight>({light.intensity, light.diffuseColor});
  } else if (light.type == lsw::geo::Light::Type::POINT_LIGHT) {
    lightEntity.add<ecs::PointLight>({.intensity = light.intensity,
                                      .linearAttenuation = light.attenuationLinear,
                                      .quadraticAttenuation = light.attenuationQuadratic,
                                      .radius = light.radius.x,
                                      .color = light.diffuseColor});
  } else {
    throw std::runtime_error("Cannot make light source for unknown Light type.");
  }

  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makeAmbientLight(std::string name, glm::vec3 color, float intensity) {
  auto lightEntity = makeMoveableEntity(std::move(name));
  lightEntity.add<ecs::AmbientLight>({.intensity = 1.0F, .color = color});
  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makePointLight(std::string name, glm::vec3 position) {
  auto lightEntity = makeMoveableEntity(std::move(name));
  lightEntity.add<ecs::PointLight>();

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::vec4(position, 1.0F);
  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makePointLight(std::string name, glm::vec3 position, ecs::PointLight pointLight) {
  auto lightEntity = makeMoveableEntity(std::move(name));
  lightEntity.add<ecs::PointLight>(std::move(pointLight));

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::vec4(position, 1.0F);
  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makeDirectionalLight(std::string name, glm::vec3 direction) {
  auto lightEntity = makeMoveableEntity(std::move(name));
  lightEntity.add<ecs::DirectionalLight>();

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::normalize(glm::vec4(direction, 1.0F));
  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makeMesh(const lsw::geo::Mesh& mesh) {
  auto meshEntity = makeMoveableEntity(mesh.name);

  meshEntity.add<lsw::geo::Mesh>(mesh);

  // Watch out here, lsw::geo::Material is a value type so we can do this.
  // It is very tricky, because we ignore the resource itself here, possibly
  // causing dangling pointers in the end
  if (m_defaultMaterial == nullptr) {
    throw std::runtime_error("No default material set, cannot create mesh");
  }
//  meshEntity.add<lsw::geo::Material>(*m_defaultMaterial);

  m_renderableComponentFactory->addRenderableComponent(m_registry, meshEntity, m_defaultMaterial->id);

  //  auto& shader = meshEntity.add<ecs::Shader>(
  //      {"assets/shaders/diffuse.vert.spv",
  //      "assets/shaders/diffuse.frag.spv"});
  //  shader.textures.emplace_back(Texture{
  //    .diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg"
  //  };

  return meshEntity;
}

SceneGraphEntity SceneGraphHelper::makeEmptyMesh(const lsw::geo::Mesh& mesh) {
  auto sge = makeMoveableEntity(mesh.name);
  auto e = sge.handle();

  m_registry.emplace<lsw::geo::Mesh>(e, mesh);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, m_defaultMaterial->id);
  return sge;
}

// SceneGraphEntity SceneGraphHelper::makeMeshInstance(const lsw::geo::Mesh &mesh) {
//  auto sge = makeEntity(mesh.name);
//  auto e = sge.id();
//
//  auto &renderable = m_registry.emplace<ecs::Renderable>(e);
//  renderable.name = mesh.name;
//
//  m_registry.emplace<lsw::geo::Mesh>(e, mesh);
//  m_registry.emplace<ecs::Shader>(
//      e, ecs::Shader{"assets/shaders/diffuse.vert.spv",
//                     "assets/shaders/diffuse.frag.spv"});
//
//  Texture &texture = m_registry.emplace<Texture>(e);
//  texture.diffuseTextureFilepath =
//  "assets/textures/textures/castle_normal.jpg";
//
//  return sge;
//}

SceneGraphEntity SceneGraphHelper::makeCurve(std::string name) {
  auto curve = makeMoveableEntity(std::move(name));

  curve.add<lsw::geo::Curve>();
//  auto& s = curve.add<lsw::geo::Material>({.name = "default curve material",
//                                           .vertexShader = "assets/shaders/default_curve.vert.spv",
//                                           .fragmentShader = "assets/shaders/default_curve.frag.spv"});

//  auto block = new gl::ColorBlock;
//  block->color = glm::vec4(0.45F, 0.52F, 0.68F, 0.0F);
//  s.setProperty("ColorBlock", block);
  // TODO: Add color block manager to material system,
  //  OR make a separate list of constantUniformBlocks that dont need management,
  //  OR whenever you look for ColorBlock manager and you cannot find one, then ignore.
  //  s.registerUniformBlock("ColorBlock", block, sizeof(ecs::ColorBlock));

  m_renderableComponentFactory->addRenderableComponent(m_registry, curve, m_defaultMaterial->id);

  return curve;
}

void SceneGraphHelper::processChildren(std::shared_ptr<const lsw::geo::SceneNode> node, SceneLoaderFlags flags, SceneGraphEntity* parent_p) {
  auto root = makeMoveableEntity(node->name);
  root.setTransform(node->transform);

  // if current processed scene node is not a root node.
  // then add it to it's parent.
  if (parent_p != nullptr) {
    parent_p->addChild(root);
  }

  // Add lights
  if (flags.lights) {
    for (auto& light : node->lights) {
      auto lightEntity = makeLight(*light);
      root.addChild(lightEntity);
    }
  }

  // Add cameras
  if (flags.cameras) {
    for (auto& camera : node->cameras) {
      auto cameraEntity = makeCamera(*camera);
      root.addChild(cameraEntity);
    }
  }

  if (flags.geometry) {
    // add mesh instances for this root
    for (auto& instance : node->meshInstances) {
      // make shader from material
      // TODO: make a material system that loads a default
      //    auto material =
      //        m_resourceManager->getResource<lsw::geo::Material>("UberMaterial");
      //
      //    (*material)->diffuseTexture = (*instance->material)->diffuseTexture;
      //
      //    (*material)->setProperty<lsw::geo::UberMaterialData>(
      //        {.diffuse = glm::vec4((*instance->material)->diffuse, 1.0F),
      //         .specular = glm::vec4((*instance->material)->specular, 1.0F),
      //         .ambient = glm::vec4((*instance->material)->ambient, 1.0F),
      //         .hasDiffuseTex = (*instance->material)->diffuseTexture !=
      //         nullptr});

      // create mesh instance mesh data
      // TODO: make mesh instance instead of copy mesh
      //    auto e = makeRenderable(*instance->mesh, instance->transform,
      //    **material);
      auto e = makeRenderable(*instance->mesh, instance->transform, instance->materialId);

      root.addChild(e);
    }
  }

  for (auto& child : node->children) {
    processChildren(child, flags, &root);
  }
}

SceneGraphEntity SceneGraphHelper::makeScene(const lsw::geo::Scene& scene, SceneLoaderFlags flags) {
  auto rootScene = makeMoveableEntity();

  // for geometry and mesh data
  processChildren(scene.rootNode(), flags, &rootScene);

  return rootScene;
}

SceneGraphEntity SceneGraphHelper::makeRenderable(const lsw::geo::Mesh& mesh, glm::mat4 transform, int materialId) {
  auto e = makeMoveableEntity(mesh.name);
  e.setTransform(std::move(transform));

  e.add<lsw::geo::Mesh>(mesh);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e.handle(), materialId);

  spdlog::info("(e: {}) Added mesh {} - with renderStateId: {}", static_cast<int>(e.handle()),
               mesh.name, m_registry.get<gl::DeferredRenderable>(e).renderStateId);
  return e;
}

SceneGraphEntity SceneGraphHelper::makeRenderable(lsw::geo::Mesh&& mesh, int materialId) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<lsw::geo::Mesh>(e, std::move(mesh));
//  m_registry.emplace<lsw::geo::Material>(e, material);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);

  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity SceneGraphHelper::makeRenderable(lsw::geo::Curve&& curve, int materialId) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<lsw::geo::Curve>(e, std::move(curve));
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);
 throw std::runtime_error("No curves supported yet");
//  return SceneGraphEntity{m_registry, e};
}

entt::entity makeTexture();

SceneGraphEntity SceneGraphHelper::makePosteffect(const std::string name, int materialId) {
  auto e = makeEntity(name);
//  e.add<lsw::geo::Material>(material);
  e.add<gl::Posteffect>();
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);
  return e;
}

SceneGraphEntity SceneGraphHelper::makeRectangularGrid(float size, float spacing) {
  auto sge = makeCurve("RectangularGrid");
  auto& curve = sge.get<lsw::geo::Curve>();

  float z = 0.0F;

  // initial center line axis
  curve.vertices.push_back(-size);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(z);

  curve.vertices.push_back(size);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(z);

  // initial center line axis
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(-size);

  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(size);

  while (z - spacing < size) {
    // positive side horizontal
    curve.vertices.push_back(-size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(z);

    curve.vertices.push_back(size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(z);

    curve.vertices.push_back(-size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-z);

    curve.vertices.push_back(size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-z);

    // positive side vertical
    curve.vertices.push_back(z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-size);

    curve.vertices.push_back(z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(size);

    curve.vertices.push_back(-z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-size);

    curve.vertices.push_back(-z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(size);

    z += spacing * 2.0F;
  }

  return sge;
}
