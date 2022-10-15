//
// Created by jlemein on 18-01-21.
//
#include "izzgl_sceneloader.h"

#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include "geo_camera.h"
#include "geo_light.h"
#include "geo_materialtemplate.h"
#include "geo_mesh.h"
#include "geo_meshinstance.h"
#include "geo_scene.h"
#include "izzgl_materialsystem.h"
#include "izzgl_texturesystem.h"

#include <deque>
#include <filesystem>

using namespace izz::gl;

SceneLoader::SceneLoader(std::shared_ptr<izz::gl::TextureSystem> textureSystem, std::shared_ptr<izz::gl::MaterialSystem> materialSystem)
  : m_textureSystem{textureSystem}
  , m_materialSystem{materialSystem} {}

std::unique_ptr<izz::geo::TextureDescription> SceneLoader::readAiTexture(const izz::geo::Scene& scene, aiTextureType ttype, const aiMaterial* aiMaterial_p,
                                                                         const std::unordered_map<std::string, aiTexture*>& embeddedTextures) const {
  std::unique_ptr<izz::geo::TextureDescription> td = nullptr;

  if (aiMaterial_p->GetTextureCount(ttype) > 0) {
    td = std::make_unique<izz::geo::TextureDescription>();
    td->type = izz::geo::PropertyType::TEXTURE_2D;

    aiString aiPath;
    aiTextureMapping textureMapping;
    aiMaterial_p->GetTexture(ttype, 0, &aiPath, &textureMapping);

    if (embeddedTextures.contains(aiPath.C_Str())) {
      auto texture_p = embeddedTextures.at(aiPath.C_Str());

      unsigned char* imageData = reinterpret_cast<unsigned char*>(texture_p->pcData);
      auto size = texture_p->mHeight != 0 ? texture_p->mWidth * texture_p->mHeight : texture_p->mWidth;

      Texture* embedded = m_textureSystem->loadEmbeddedTexture(aiPath.C_Str(), imageData, size, std::string(".") + texture_p->achFormatHint);
      td->path = embedded->path;
    } else {
      // some models are exported with OS specific path separators.
      // Replace these separators with preferred path separator.
      std::string path = aiPath.C_Str();
      std::replace(path.begin(), path.end(), '\\', std::filesystem::path::preferred_separator);
      td->path = scene.m_dir / path;
    }

    td->name = "";

    switch (ttype) {
      case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
        td->hint = izz::geo::TextureHint::ROUGHNESS_MAP;
        td->name = "roughnessMap";
        break;
      case aiTextureType::aiTextureType_NORMALS:
        td->hint = izz::geo::TextureHint::NORMAL_MAP;
        td->name = "normalMap";
        break;
      case aiTextureType::aiTextureType_SPECULAR:
        td->hint = izz::geo::TextureHint::SPECULAR_MAP;
        td->name = "specularMap";
        break;
      case aiTextureType::aiTextureType_DIFFUSE:
        td->hint = izz::geo::TextureHint::DIFFUSE_MAP;
        td->name = "diffuseMap";
        break;
      case aiTextureType::aiTextureType_HEIGHT:
        td->hint = izz::geo::TextureHint::HEIGHT_MAP;
        td->name = "heightMap";
        break;
      default:
        spdlog::debug("Could not map texture type to Izzy render's texture type (model: '{}')", scene.m_path.string());
        break;
    }
  }

  return td;
}

void SceneLoader::readTextures(const izz::geo::Scene& scene, const aiScene* aiScene_p, const aiMaterial* aiMaterial_p, izz::geo::MaterialTemplate& material) {
  std::array<aiTextureType, 4> textureTypes{aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR, aiTextureType_DIFFUSE_ROUGHNESS};

  aiString textureFile;
  aiMaterial_p->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureFile);
  spdlog::warn("texture file: {}", textureFile.C_Str());

  std::unordered_map<std::string, aiTexture*> embeddedTextures = {};
  for (int i=0; i<aiScene_p->mNumTextures; ++i) {
    auto filename = aiScene_p->mTextures[i]->mFilename.C_Str();
    embeddedTextures[filename] = aiScene_p->mTextures[i];
  }

  for (auto aiTextureType : textureTypes) {
    if (auto pTextureDescription = readAiTexture(scene, aiTextureType, aiMaterial_p, embeddedTextures)) {

      // overwrite the stored texture if the texture inside the scene file matches the same hint
      for (auto& [name, storedTexture] : material.textures) {
        if (pTextureDescription->hint == storedTexture.hint) {
          pTextureDescription->name = name;
          material.textures[name] = *pTextureDescription;
        }
      }
    }
  }
}

void SceneLoader::readMaterials(const aiScene* scene_p, izz::geo::Scene& scene) {
  for (int i = 0; i < scene_p->mNumMaterials; ++i) {
    aiMaterial* aiMaterial = scene_p->mMaterials[i];

    std::string name = aiMaterial->GetName().C_Str();
    izz::geo::MaterialTemplate materialDescription = m_materialSystem->getMaterialTemplate(name);

    spdlog::debug("Read material {} -- (vertex shader: {})", name, materialDescription.vertexShader);

    spdlog::debug("Number of embedded textures: {}", scene_p->mNumTextures);
    for (int j=0; j<scene_p->mNumTextures; ++j) {
      spdlog::debug("Embedded texture: {}, {} x {} -- {}", scene_p->mTextures[0]->mFilename.C_Str(), scene_p->mTextures[0]->mWidth, scene_p->mTextures[0]->mHeight, std::string(scene_p->mTextures[0]->achFormatHint));
    }
    readTextures(scene, scene_p, aiMaterial, materialDescription);

    // overwrite settings with scene file properties
    aiColor3D color(0.f, 0.f, 0.f);
    aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    materialDescription.diffuseColor.r = color.r;
    materialDescription.diffuseColor.g = color.g;
    materialDescription.diffuseColor.b = color.b;

    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
    materialDescription.specularColor.r = color.r;
    materialDescription.specularColor.g = color.g;
    materialDescription.specularColor.b = color.b;

    aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
    materialDescription.ambientColor.r = color.r;
    materialDescription.ambientColor.g = color.g;
    materialDescription.ambientColor.b = color.b;

    // a loaded scene maintains own copy of material descriptions.
    scene.m_materials.push_back(materialDescription);
  }
}

void SceneLoader::readMeshes(const aiScene* scene_p, izz::geo::Scene& scene) {
  scene.m_meshes.reserve(scene_p->mNumMeshes);

  for (unsigned int n = 0U; n < scene_p->mNumMeshes; ++n) {
    auto mesh = std::make_shared<izz::geo::Mesh>();
    const aiMesh* mesh_p = scene_p->mMeshes[n];

    mesh->name = mesh_p->mName.C_Str();
    mesh->polygonMode = izz::geo::PolygonMode::kTriangles;
    mesh->materialId = mesh_p->mMaterialIndex; // refers to local material descriptions
    const auto& materialDescription = scene.m_materials.at(mesh->materialId);
    spdlog::debug("Reading mesh {}: has material {} - {}", n, mesh_p->mName.C_Str(), materialDescription.vertexShader);

    for (int i = 0; i < mesh_p->mNumVertices; ++i) {
      auto pVertex = mesh_p->mVertices[i];
      mesh->vertices.push_back(pVertex.x);
      mesh->vertices.push_back(pVertex.y);
      mesh->vertices.push_back(pVertex.z);

      if (mesh_p->HasNormals()) {
        auto pNormal = mesh_p->mNormals[i];
        mesh->normals.push_back(pNormal.x);
        mesh->normals.push_back(pNormal.y);
        mesh->normals.push_back(pNormal.z);
      }

      aiVector3D ZERO3D = {0, 0, 0};
      auto uv = mesh_p->HasTextureCoords(0) ? mesh_p->mTextureCoords[0][i] : ZERO3D;
      mesh->uvs.push_back(uv.x);
      mesh->uvs.push_back(uv.y);
    }

    for (int f = 0; f < mesh_p->mNumFaces; ++f) {
      auto face = mesh_p->mFaces[f];
      mesh->indices.push_back(face.mIndices[0]);
      mesh->indices.push_back(face.mIndices[1]);
      mesh->indices.push_back(face.mIndices[2]);
    }

    scene.m_meshes.emplace_back(mesh);
  }
}

void SceneLoader::readHierarchy(const aiScene* scene_p, izz::geo::Scene& scene) {
  using ChildParent = std::pair<aiNode*, std::shared_ptr<izz::geo::SceneNode>>;
  std::deque<ChildParent> queue;

  queue.push_back(std::make_pair<>(scene_p->mRootNode, nullptr));

  // Queue is filled with entities, starting with root node.
  // Every time a node is processed.

  while (!queue.empty()) {
    auto [node_p, parent_p] = queue.front();
    queue.pop_front();

    auto node = std::make_shared<izz::geo::SceneNode>();
    if (parent_p != nullptr) {
      parent_p->children.push_back(node);
    } else {
      scene.m_rootNode = node;
    }

    node->name = std::string{node_p->mName.C_Str()};

    // assimp stores the transformations in
    std::memcpy(&node->transform[0][0], &node_p->mTransformation.a1, 16 * sizeof(ai_real));
    node->transform = glm::transpose(node->transform);

    // loops through all mesh instances of this node
    node->meshInstances.reserve(node_p->mNumMeshes);
    for (int i = 0U; i < node_p->mNumMeshes; ++i) {
      auto meshInstance = std::make_shared<izz::geo::MeshInstance>();

      auto mesh = scene.m_meshes[node_p->mMeshes[i]];

      //      if (node_p->mMetaData != nullptr) {
      //        for (int k = 0; k < node_p->mMetaData->mNumProperties; ++k) {
      //          spdlog::debug("\t\t{}", node_p->mMetaData->mKeys[k].C_Str());
      //        }
      //      }
      meshInstance->mesh = mesh;
      meshInstance->name = std::string{node_p->mName.C_Str()};
      meshInstance->materialId = mesh->materialId;
      meshInstance->transform = glm::mat4(1.0F);

      node->meshInstances.push_back(meshInstance);
    }

    // register name
    scene.registerSceneNode(node);

    // add the children for the next round
    for (int i = 0; i < node_p->mNumChildren; ++i) {
      queue.push_back(std::make_pair<>(node_p->mChildren[i], node));
    }
  }
}

izz::geo::Scene::SceneNodeIterable izz::geo::Scene::getSceneNodesByName(const std::string& name) {
  return m_sceneNodes.count(name) > 0 ? m_sceneNodes.at(name) : SceneNodeIterable{};
}

void izz::geo::Scene::registerSceneNode(std::shared_ptr<izz::geo::SceneNode> node) {
  m_sceneNodes[node->name].push_back(node);

  if (m_sceneNodes.at(node->name).size() > 1) {
    spdlog::warn("Node with name '{}' has more than 1 entry. Is this correct?", node->name);
  }
}

void SceneLoader::readCameras(const aiScene* scene_p, izz::geo::Scene& scene) {
  for (int i = 0; i < scene_p->mNumCameras; ++i) {
    auto aiCamera_p = scene_p->mCameras[i];

    auto camera = std::make_shared<izz::geo::Camera>();
    camera->name = aiCamera_p->mName.C_Str();
    camera->fovx = aiCamera_p->mHorizontalFOV;
    camera->aspect = aiCamera_p->mAspect;
    camera->position = glm::vec3(aiCamera_p->mPosition.x, aiCamera_p->mPosition.y, aiCamera_p->mPosition.z);
    camera->lookAt = glm::vec3(aiCamera_p->mLookAt.x, aiCamera_p->mLookAt.y, aiCamera_p->mLookAt.z);
    camera->up = glm::vec3(aiCamera_p->mUp.x, aiCamera_p->mUp.y, aiCamera_p->mUp.z);
    camera->near = aiCamera_p->mClipPlaneNear;
    camera->far = aiCamera_p->mClipPlaneFar;
    // not sure about this
    camera->fovy = camera->fovx / camera->aspect;

    auto cameraNodes = scene.getSceneNodesByName(camera->name);
    if (cameraNodes.empty()) {
      scene.m_rootNode->cameras.emplace_back(camera);
    } else {
      for (auto node : cameraNodes) {
        node->cameras.emplace_back(camera);
      }
    }

    scene.cameras().emplace_back(camera);
  }
}

void SceneLoader::readLights(const aiScene* aiScene, izz::geo::Scene& scene) {
  for (int i = 0; i < aiScene->mNumLights; ++i) {
    auto aiLight = aiScene->mLights[i];
    auto light = std::make_shared<izz::geo::Light>();
    light->name = std::string{aiLight->mName.C_Str()};

    // diffuse color encodes wattage
    light->diffuseColor = glm::vec3(aiLight->mColorDiffuse.r, aiLight->mColorDiffuse.g, aiLight->mColorDiffuse.b);
    // TODO: we use blender standard. 1000W is considered intensity 1 - this is chosen arbitrarily.
    //  investigate how to improve, maybe HDR eventually
    light->intensity = glm::length(light->diffuseColor);
    light->diffuseColor = glm::normalize(light->diffuseColor);

    light->specularColor = glm::vec3(aiLight->mColorSpecular.r, aiLight->mColorSpecular.g, aiLight->mColorSpecular.b);
    light->ambientColor = glm::vec3(aiLight->mColorAmbient.r, aiLight->mColorAmbient.g, aiLight->mColorAmbient.b);

    light->upVector = glm::vec3{aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z};
    light->attenuationQuadratic = aiLight->mAttenuationQuadratic;
    light->position = glm::vec3(aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z);
    light->radius = glm::vec2(aiLight->mSize.x, aiLight->mSize.y);

    switch (aiLight->mType) {
      case aiLightSource_POINT:
        light->type = izz::geo::Light::Type::POINT_LIGHT;
        break;

      case aiLightSource_DIRECTIONAL:
        light->type = izz::geo::Light::Type::DIRECTIONAL_LIGHT;
        light->position = glm::vec3(aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z);
        light->position *= -1;
        break;

      case aiLightSource_AMBIENT:
        light->type = izz::geo::Light::Type::AMBIENT_LIGHT;
        break;

      case aiLightSource_SPOT:
        light->type = izz::geo::Light::Type::SPOT_LIGHT;
        break;

      default:
        std::cerr << "Unknown light source type. Ignoring light source type.\n";
        continue;
    }

    // light belongs to a transform node
    auto lightNodes = scene.getSceneNodesByName(light->name);
    if (lightNodes.empty()) {
      scene.m_rootNode->lights.emplace_back(light);
    } else {
      for (auto node : lightNodes) {
        node->lights.emplace_back(light);
      }
    }

    scene.m_lights.emplace_back(light);
  }
}

void readTextures(const aiScene* scene_p, izz::geo::Scene& scene) {
  for (int i = 0; i < scene_p->mNumTextures; ++i) {
    auto aiTexture = scene_p->mTextures[i];
    std::cerr << "Could not load texture: " << aiTexture->mFilename.C_Str() << ": not yet implemented\n";
  }
}

std::unique_ptr<izz::geo::Scene> SceneLoader::loadScene(std::filesystem::path path) {
  izz::geo::Scene scene;
  scene.m_path = path;
  scene.m_dir = scene.m_path.parent_path();

  Assimp::Importer m_importer;
  const aiScene* aiScene_p{nullptr};

  aiScene_p = m_importer.ReadFile(path, aiProcess_Triangulate);
  if (!aiScene_p) {
    throw std::runtime_error(fmt::format("Failed to load scene file: {}", path.string()));
  }
  readMaterials(aiScene_p, scene);
  //  readEmbeddedTextures(aiScene_p, scene);

  readMeshes(aiScene_p, scene);
  readHierarchy(aiScene_p, scene);

  // should come after reading of hierarchy
  readLights(aiScene_p, scene);
  readCameras(aiScene_p, scene);

  return std::make_unique<izz::geo::Scene>(std::move(scene));
}
