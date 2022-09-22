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
#include "geo_camera.h"
#include "geo_curve.h"
#include "geo_light.h"
#include "geo_mesh.h"
#include "geo_meshinstance.h"
#include "geo_scene.h"
#include "gl_deferredrenderer.h"
#include "izzgl_material.h"
#include "izzgl_materialsystem.h"

using namespace izz;
using namespace izz;
using namespace izz::ecs;

SceneGraphHelper::SceneGraphHelper(entt::registry& registry, std::unique_ptr<RenderableComponentFactory> renderableComponentFactory,
                                   std::shared_ptr<izz::gl::MaterialSystem> materialSystem, std::shared_ptr<izz::gl::MeshSystem> meshSystem)
  : m_registry{registry}
  , m_renderableComponentFactory{std::move(renderableComponentFactory)}
  , m_materialSystem{materialSystem}
  , m_meshSystem{meshSystem} {}

void SceneGraphHelper::setDefaultMaterial(std::shared_ptr<izz::gl::Material> material) {
  m_defaultMaterial = material;
}

SceneGraphEntity SceneGraphHelper::addGeometry(izz::geo::Mesh mesh, int materialId) {
  auto e = makeMoveableEntity(mesh.name);
  e.add<izz::geo::Mesh>(mesh);
  auto& meshBuffer = m_meshSystem->createMeshBuffer(mesh);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId, meshBuffer.id);

  // TODO: make sure we store a shared ptr instead of a copy.
  //  shared materials offer option to share materials.
  //  e.add<izz::gl::Material>(*mat);

  return e;
}

// SceneGraphEntity SceneGraphHelper::addGeometry(lsw::geo::Mesh mesh, geo::cEffect effect) {
//   auto e = makeMoveableEntity(mesh.name);
//   e.add<lsw::geo::Mesh>(mesh);
//   // TODO: make sure we store a shared ptr instead of a copy.
//   //  shared materials offer option to share materials.
//   e.add<geo::cEffect>(effect);
//   m_renderableComponentFactory->addRenderableComponent(m_registry, e, );
//
//   return e;
// }

// SceneGraphEntity SceneGraphHelper::addGeometry(lsw::geo::Mesh&& mesh, izz::gl::Material&& material) {
//   auto e = makeEntity(mesh.name);
//   e.add<RENDERABLE_COMPONENT>();
//   e.add<lsw::geo::Mesh>(std::forward<lsw::geo::Mesh>(mesh));
//   e.add<izz::gl::Material>(std::forward<izz::gl::Material>(material));
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

SceneGraphEntity SceneGraphHelper::makeCamera(const izz::geo::Camera& geoCamera) {
  auto cameraEntity = makeMoveableEntity(geoCamera.name);
  ecs::Camera camera{.fovx = geoCamera.fovx, .aspect = geoCamera.aspect, .zNear = geoCamera.near, .zFar = geoCamera.far};
  cameraEntity.add<ecs::Camera>(std::move(camera));

  auto& transform = cameraEntity.get<Transform>();
  transform.localTransform = glm::lookAt(geoCamera.position, geoCamera.lookAt, geoCamera.up);

  return cameraEntity;
}

SceneGraphEntity SceneGraphHelper::makeLight(const izz::geo::Light& light) {
  auto lightEntity = makeMoveableEntity(light.name);

  auto& transform = lightEntity.get<Transform>();
  transform.localTransform[3] = glm::vec4(light.position, 1.0F);

  if (light.type == izz::geo::Light::Type::DIRECTIONAL_LIGHT) {
    transform.localTransform[3].w = 0.0F;
    lightEntity.add<ecs::DirectionalLight>({light.intensity, light.diffuseColor});
  } else if (light.type == izz::geo::Light::Type::AMBIENT_LIGHT) {
    lightEntity.add<ecs::AmbientLight>({light.intensity, light.diffuseColor});
  } else if (light.type == izz::geo::Light::Type::POINT_LIGHT) {
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
  lightEntity.add<ecs::AmbientLight>({.intensity = intensity, .color = color});
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

SceneGraphEntity SceneGraphHelper::makeSpotLightFromLookAt(std::string name, glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
  auto lightEntity = makeMoveableEntity(std::move(name));
  auto& spotlight = lightEntity.add<ecs::SpotLight>();

  // TODO: create local transform by direct matrix calculation. Inverse should not be needed
  //  auto w = glm::normalize(center - eye);
  //  auto v = glm::cross(w, up);
  //  auto u = glm::cross(v, w);
  //  transform.localTransform = glm::mat4(glm::vec4(u, 0), glm::vec4(v, 0), glm::vec4(w, 0), glm::vec4(eye, 1));

  auto& transform = lightEntity.get<Transform>();
  transform.localTransform = glm::inverse(glm::lookAt(eye, center, up));

  return lightEntity;
}

SceneGraphEntity SceneGraphHelper::makeMesh(const izz::geo::Mesh& mesh) {
  auto meshEntity = makeMoveableEntity(mesh.name);

  auto& meshBuffer = m_meshSystem->createMeshBuffer(mesh);
  meshEntity.add<izz::geo::Mesh>(mesh);

  // Watch out here, izz::gl::Material is a value type so we can do this.
  // It is very tricky, because we ignore the resource itself here, possibly
  // causing dangling pointers in the end
  if (m_defaultMaterial == nullptr) {
    throw std::runtime_error("No default material set, cannot create mesh");
  }
  //  meshEntity.add<izz::gl::Material>(*m_defaultMaterial);

  m_renderableComponentFactory->addRenderableComponent(m_registry, meshEntity, m_defaultMaterial->id, meshBuffer.id);

  //  auto& shader = meshEntity.add<ecs::Shader>(
  //      {"assets/shaders/diffuse.vert.spv",
  //      "assets/shaders/diffuse.frag.spv"});
  //  shader.textures.emplace_back(Texture{
  //    .diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg"
  //  };

  return meshEntity;
}

SceneGraphEntity SceneGraphHelper::makeEmptyMesh(const izz::geo::Mesh& mesh) {
  auto sge = makeMoveableEntity(mesh.name);
  auto e = sge.handle();

  m_registry.emplace<izz::geo::Mesh>(e, mesh);
  const auto& meshBuffer = m_meshSystem->createMeshBuffer(mesh);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, m_defaultMaterial->id, meshBuffer.id);
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

  curve.add<izz::geo::Curve>();
  //  auto& s = curve.add<izz::gl::Material>({.name = "default curve material",
  //                                           .vertexShader = "assets/shaders/default_curve.vert.spv",
  //                                           .fragmentShader = "assets/shaders/default_curve.frag.spv"});

  //  auto block = new gl::ColorBlock;
  //  block->color = glm::vec4(0.45F, 0.52F, 0.68F, 0.0F);
  //  s.setProperty("ColorBlock", block);
  // TODO: Add color block manager to material system,
  //  OR make a separate list of constantUniformBlocks that dont need management,
  //  OR whenever you look for ColorBlock manager and you cannot find one, then ignore.
  //  s.registerUniformBlock("ColorBlock", block, sizeof(ecs::ColorBlock));

  //  auto& meshBuffer = m_meshSystem->createMeshBuffer();
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, curve, m_defaultMaterial->id, meshBuffer.id);

  return curve;
}

void SceneGraphHelper::processChildren(const izz::geo::Scene& scene, std::shared_ptr<const izz::geo::SceneNode> node, SceneLoaderFlags flags,
                                       SceneGraphEntity* parent_p) {
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
      auto& materialDescription = scene.m_materials[instance->materialId];
      auto& material = m_materialSystem->createMaterial(materialDescription);

      auto meshBuffer = m_meshSystem->createMeshBuffer(*instance->mesh);

      // TODO: make mesh instance instead of copy mesh
      //    auto e = makeRenderable(*instance->mesh, instance->transform,
      //    **material);

      auto e = makeRenderable(instance->name, meshBuffer, instance->transform, material.id);

      root.addChild(e);
    }
  }

  for (auto& child : node->children) {
    processChildren(scene, child, flags, &root);
  }
}

SceneGraphEntity SceneGraphHelper::makeScene(const izz::geo::Scene& scene, SceneLoaderFlags flags) {
  spdlog::debug("{}: instantiating scene objects ({})", ID, scene.m_path.string());

  auto rootScene = makeMoveableEntity();

  // for geometry and mesh data
  processChildren(scene, scene.rootNode(), flags, &rootScene);

  return rootScene;
}

SceneGraphEntity SceneGraphHelper::makeRenderable(std::string name, const izz::gl::MeshBuffer& meshBuffer, glm::mat4 transform, MaterialId materialId) {
  auto e = makeMoveableEntity(name);
  e.setTransform(std::move(transform));

  //  e.add<izz::gl::MeshBuffer>(meshBuffer);

  m_renderableComponentFactory->addRenderableComponent(m_registry, e.handle(), materialId, meshBuffer.id);

  spdlog::info("(e: {}) Added mesh {} with material id: {}, mesh buffer id: {}", static_cast<int>(e.handle()), name,
               m_registry.get<gl::DeferredRenderable>(e).materialId, m_registry.get<gl::DeferredRenderable>(e).meshBufferId);
  return e;
}

SceneGraphEntity SceneGraphHelper::makeRenderable(izz::gl::MeshBuffer&& meshBuffer, MaterialId materialId) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  //  m_registry.emplace<izz::gl::MeshBuffer>(e, std::move(mesh));
  //  m_registry.emplace<izz::gl::Material>(e, material);
  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId, meshBuffer.id);

  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity SceneGraphHelper::makeRenderable(izz::geo::Curve&& curve, MaterialId materialId) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<izz::geo::Curve>(e, std::move(curve));
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId, meshBuffer.id);
  throw std::runtime_error("No curves supported yet");
  //  return SceneGraphEntity{m_registry, e};
}

entt::entity makeTexture();

SceneGraphEntity SceneGraphHelper::makePosteffect(const std::string name, int materialId) {
  auto e = makeEntity(name);
  //  e.add<izz::gl::Material>(material);
  e.add<gl::Posteffect>();
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);
  throw std::runtime_error("Not supported yet");
  //  return e;
}

SceneGraphEntity SceneGraphHelper::makeRectangularGrid(float size, float spacing) {
  auto sge = makeCurve("RectangularGrid");
  auto& curve = sge.get<izz::geo::Curve>();

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
