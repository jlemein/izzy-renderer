#include <izz_scenegraphentity.h>
#include <izzgl_entityfactory.h>

#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_texture.h>
#include <ecs_transform.h>
#include "geo_shapeutil.h"
#include "izz_relationship.h"

#include <geo_camera.h>
#include <geo_curve.h>
#include <geo_light.h>
#include <geo_mesh.h>
#include <geo_meshinstance.h>
#include <geo_scene.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgl_rendersystem.h>
#include <spdlog/spdlog.h>

using namespace izz;
using namespace izz;
using namespace izz::ecs;

EntityFactory::EntityFactory(entt::registry& registry, gl::RenderSystem& renderSystem,
                             izz::gl::MaterialSystem& materialSystem, izz::gl::MeshSystem& meshSystem)
  : m_registry{registry}
  , m_renderSystem{renderSystem}
  , m_materialSystem{materialSystem}
  , m_meshSystem{meshSystem} {}

void EntityFactory::setDefaultMaterial(std::shared_ptr<izz::gl::Material> material) {
  m_defaultMaterial = material;
}

SceneGraphEntity EntityFactory::addGeometry(izz::geo::Mesh mesh, int materialId) {
  auto e = makeMovableEntity(mesh.name);
  e.add<izz::geo::Mesh>(mesh);
  auto& meshBuffer = m_meshSystem.createVertexBuffer(mesh);

  e.add(izz::Geometry{.materialId = materialId, .vertexBufferId = meshBuffer.id, .aabb=geo::ShapeUtil::computeBoundingBox(mesh)});
  m_renderSystem.onGeometryAdded(e, m_defaultRenderStrategy);

  //  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId, meshBuffer.id);

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

SceneGraphEntity EntityFactory::makeEntity(std::string name) {
  auto e{m_registry.create()};
  if (name.empty()) {
    name = std::string{"Node #"} + std::to_string(NEXT_ENTITY_ID++);
  }
  m_registry.emplace<Name>(e, name);
  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity EntityFactory::makeMovableEntity(std::string name, glm::vec3 position) {
  auto e{m_registry.create()};
  if (name.empty()) {
    name = std::string{"Node #"} + std::to_string(NEXT_ENTITY_ID++);
  }
  m_registry.emplace<Name>(e, name);
  Transform t;
  t.localTransform[3] = glm::vec4(position, 1.0F);
  m_registry.emplace<Transform>(e, t);
  m_registry.emplace<izz::Relationship>(e);
  return SceneGraphEntity{m_registry, e};
}

SceneGraphEntity EntityFactory::makeCamera(std::string name, float zDistance, float fovx, float aspect, float zNear, float zFar) {
  auto cameraEntity = makeMovableEntity(std::move(name));
  ecs::Camera camera{.fovx = fovx, .aspect = aspect, .zNear = zNear, .zFar = zFar};
  cameraEntity.add<ecs::Camera>(std::move(camera));
  cameraEntity.get<ecs::Transform>().localTransform[3] = glm::vec4(0.0F, 0.0F, zDistance, 1.0F);

  return cameraEntity;
}

SceneGraphEntity EntityFactory::makeCamera(const izz::geo::Camera& geoCamera) {
  auto cameraEntity = makeMovableEntity(geoCamera.name);
  ecs::Camera camera{.fovx = geoCamera.fovx, .aspect = geoCamera.aspect, .zNear = geoCamera.near, .zFar = geoCamera.far};
  cameraEntity.add<ecs::Camera>(std::move(camera));

  auto& transform = cameraEntity.get<Transform>();
  transform.localTransform = glm::lookAt(geoCamera.position, geoCamera.lookAt, geoCamera.up);

  return cameraEntity;
}

SceneGraphEntity EntityFactory::makeLight(const izz::geo::Light& light) {
  auto lightEntity = makeMovableEntity(light.name);

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

SceneGraphEntity EntityFactory::makeAmbientLight(std::string name, glm::vec3 color, float intensity) {
  auto lightEntity = makeMovableEntity(std::move(name));
  lightEntity.add<ecs::AmbientLight>({.intensity = intensity, .color = color});
  return lightEntity;
}

SceneGraphEntity EntityFactory::makePointLight(std::string name, glm::vec3 position) {
  auto lightEntity = makeMovableEntity(std::move(name));
  lightEntity.add<ecs::PointLight>();

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::vec4(position, 1.0F);
  return lightEntity;
}

SceneGraphEntity EntityFactory::makePointLight(std::string name, glm::vec3 position, ecs::PointLight pointLight) {
  auto lightEntity = makeMovableEntity(std::move(name));
  lightEntity.add<ecs::PointLight>(std::move(pointLight));

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::vec4(position, 1.0F);
  return lightEntity;
}

SceneGraphEntity EntityFactory::makeDirectionalLight(std::string name, glm::vec3 direction) {
  auto lightEntity = makeMovableEntity(std::move(name));
  lightEntity.add<ecs::DirectionalLight>();

  // different between point and directional light is in the w component.
  lightEntity.get<Transform>().localTransform[3] = glm::normalize(glm::vec4(direction, 1.0F));
  return lightEntity;
}

SceneGraphEntity EntityFactory::makeSpotLightFromLookAt(std::string name, glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
  auto lightEntity = makeMovableEntity(std::move(name));
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

SceneGraphEntity EntityFactory::makeMesh(const izz::geo::Mesh& mesh) {
  auto meshEntity = makeMovableEntity(mesh.name);

  auto& meshBuffer = m_meshSystem.createVertexBuffer(mesh);
  meshEntity.add<izz::geo::Mesh>(mesh);

  // Watch out here, izz::gl::Material is a value type so we can do this.
  // It is very tricky, because we ignore the resource itself here, possibly
  // causing dangling pointers in the end
  if (m_defaultMaterial == nullptr) {
    throw std::runtime_error("No default material set, cannot create mesh");
  }
  //  meshEntity.add<izz::gl::Material>(*m_defaultMaterial);

  meshEntity.add(izz::Geometry{.materialId = m_defaultMaterial->id, .vertexBufferId = meshBuffer.id});
  m_renderSystem.onGeometryAdded(meshEntity, m_defaultRenderStrategy);

  //  m_renderableComponentFactory->addRenderableComponent(m_registry, meshEntity, m_defaultMaterial->id, meshBuffer.id);

  //  auto& shader = meshEntity.add<ecs::Shader>(
  //      {"assets/shaders/diffuse.vert.spv",
  //      "assets/shaders/diffuse.frag.spv"});
  //  shader.textures.emplace_back(Texture{
  //    .diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg"
  //  };

  return meshEntity;
}

SceneGraphEntity EntityFactory::makeEmptyMesh(const izz::geo::Mesh& mesh) {
  auto entity = makeMovableEntity(mesh.name);
  entity.add(mesh);
  const auto& meshBuffer = m_meshSystem.createVertexBuffer(mesh);

  entity.add(izz::Geometry{.materialId = m_defaultMaterial->id, .vertexBufferId = meshBuffer.id});
  m_renderSystem.onGeometryAdded(entity, m_defaultRenderStrategy);

  return entity;
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

SceneGraphEntity EntityFactory::makeCurve(std::string name) {
  auto curve = makeMovableEntity(std::move(name));

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

  //  auto& meshBuffer = m_meshSystem.createMeshBuffer();
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, curve, m_defaultMaterial->id, meshBuffer.id);

  return curve;
}

void EntityFactory::processChildren(const izz::geo::Scene& scene, std::shared_ptr<const izz::geo::SceneNode> node, SceneLoaderFlags flags,
                                    SceneGraphEntity* parent_p, gl::RenderStrategy renderStrategy) {
  auto root = makeMovableEntity(node->name);
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
      auto& material = m_materialSystem.createMaterial(materialDescription);
      auto vertexBuffer = m_meshSystem.createVertexBuffer(*instance->mesh);
      izz::Geometry geometry {.materialId = material.id, .vertexBufferId=vertexBuffer.id, .aabb=instance->aabb};
      auto e = makeRenderable(instance->name, geometry, instance->transform, renderStrategy);

      root.addChild(e);
    }
  }

  for (auto& child : node->children) {
    processChildren(scene, child, flags, &root, renderStrategy);
  }
}

SceneGraphEntity EntityFactory::makeScene(const izz::geo::Scene& scene, SceneLoaderFlags flags, gl::RenderStrategy renderStrategy) {
  spdlog::debug("{}: instantiating scene objects ({})", ID, scene.m_path.string());

  auto rootScene = makeMovableEntity(scene.m_path.filename());

  // for geometry and mesh data
  processChildren(scene, scene.rootNode(), flags, &rootScene, renderStrategy);

  return rootScene;
}

SceneGraphEntity EntityFactory::makeRenderable(std::string name, izz::Geometry geometry, glm::mat4 transform,
                                               gl::RenderStrategy strategy) {
  auto e = makeMovableEntity(name);
  e.setTransform(std::move(transform));

  if (strategy == gl::RenderStrategy::UNDEFINED) {
    strategy = m_defaultRenderStrategy;
  }

  // todo: remove next line
  e.add<Geometry>(geometry);
  m_renderSystem.onGeometryAdded(e, strategy);

  spdlog::info("(e: {}) Added mesh {} with material id: {}, mesh buffer id: {}", static_cast<int>(e.handle()), name,
               m_registry.get<izz::Geometry>(e.handle()).materialId, geometry.vertexBufferId);
  return e;
}

SceneGraphEntity EntityFactory::makeGeometry(std::string name, izz::Geometry geometry, gl::RenderStrategy strategy) {
  return makeRenderable(name, geometry, glm::mat4(1.0), strategy);
}

SceneGraphEntity EntityFactory::makeGeometry(izz::Geometry geometry, gl::RenderStrategy strategy) {
  return makeRenderable("<unnamed>", geometry, glm::mat4(1.0), strategy);
}

SceneGraphEntity EntityFactory::makeRenderable(izz::Geometry geometry, gl::RenderStrategy strategy) {
  return makeRenderable("<unnamed>", geometry, glm::mat4(1.0), strategy);
}

SceneGraphEntity EntityFactory::makeRenderable(izz::geo::Curve&& curve, MaterialId materialId, gl::RenderStrategy strategy) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<izz::geo::Curve>(e, std::move(curve));
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId, meshBuffer.id);
  throw std::runtime_error("No curves supported yet");
  //  return SceneGraphEntity{m_registry, e};
}

entt::entity makeTexture();

SceneGraphEntity EntityFactory::makePosteffect(const std::string name, int materialId) {
  auto e = makeEntity(name);
  //  e.add<izz::gl::Material>(material);
  e.add<gl::Posteffect>();
  //  m_renderableComponentFactory->addRenderableComponent(m_registry, e, materialId);
  throw std::runtime_error("Not supported yet");
  //  return e;
}

void EntityFactory::setDefaultRenderStrategy(gl::RenderStrategy renderStrategy) {
  m_defaultRenderStrategy = renderStrategy;
}

SceneGraphEntity EntityFactory::makeRectangularGrid(float size, float spacing) {
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
