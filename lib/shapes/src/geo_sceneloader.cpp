//
// Created by jlemein on 18-01-21.
//
#include <geo_mesh.h>
#include <geo_sceneloader.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <deque>
#include <geo_meshinstance.h>
#include <geo_transform.h>
#include <spdlog/spdlog.h>

using namespace affx;
using namespace affx::geo;

void SceneLoader::readMaterials(const aiScene *scene_p, Scene &scene) {
  for (int i=0; i<scene_p->mNumMaterials; ++i) {
    aiMaterial *mat_p = scene_p->mMaterials[i];

    auto material = std::make_shared<Material>();
    material->name = mat_p->GetName().C_Str();
//    mat_p->GetTexture(aiTextureType_DIFFUSE)

    aiColor3D color (0.f,0.f,0.f);
    mat_p->Get(AI_MATKEY_COLOR_DIFFUSE,color);
    material->diffuse.r = color.r;
    material->diffuse.g = color.g;
    material->diffuse.b = color.b;
    std::cout << material->name << " has diffuse: " << color.r << " " << color.g << " " << color.b << std::endl;

    mat_p->Get(AI_MATKEY_COLOR_SPECULAR,color);
    material->specular.r = color.r;
    material->specular.g = color.g;
    material->specular.b = color.b;

    mat_p->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material->ambient.r = color.r;
    material->ambient.g = color.g;
    material->ambient.b = color.b;

    scene.m_materials.emplace_back(std::move(material));
  }
}


void SceneLoader::readMeshes(const aiScene *scene_p, Scene &scene) {
  scene.m_meshes.reserve(scene_p->mNumMeshes);

  for (unsigned int n = 0U; n < scene_p->mNumMeshes; ++n) {
    auto mesh = std::make_shared<Mesh>();
    const aiMesh *mesh_p = scene_p->mMeshes[n];
    spdlog::debug("Reading mesh {}: {}", n, mesh_p->mName.C_Str());

    mesh->name = mesh_p->mName.C_Str();
    mesh->polygonMode = PolygonMode::kTriangles;

    for (int i = 0; i < mesh_p->mNumVertices; ++i) {
      auto pVertex = mesh_p->mVertices[i];
      mesh->vertices.push_back(pVertex.x);
      mesh->vertices.push_back(pVertex.y);
      mesh->vertices.push_back(pVertex.z);

      auto pNormal = mesh_p->mNormals[i];
      mesh->normals.push_back(pNormal.x);
      mesh->normals.push_back(pNormal.y);
      mesh->normals.push_back(pNormal.z);

      mesh->material = scene.m_materials[mesh_p->mMaterialIndex];

      aiVector3D ZERO3D = {0, 0, 0};
      auto uv =
          mesh_p->HasTextureCoords(0) ? mesh_p->mTextureCoords[0][i] : ZERO3D;
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

void SceneLoader::readHierarchy(const aiScene *scene_p, Scene &scene) {
  using ChildParent = std::pair<aiNode*, std::shared_ptr<SceneNode>>;
  std::deque<ChildParent> queue;

  queue.push_back(std::make_pair<>(scene_p->mRootNode, nullptr));

  std::cout << "Mesh count in scene: " << scene.m_meshes.size() << std::endl;
  // Queue is filled with entities, starting with root node.
  // Every time a node is processed.

  while (!queue.empty()) {
    auto [node_p, parent_p] = queue.front();
    queue.pop_front();

    auto node = std::make_shared<SceneNode>();
    if (parent_p != nullptr) {
      parent_p->children.push_back(node);
    } else {
      scene.m_rootNode = node;
    }

    node->name = std::string{node_p->mName.C_Str()};
    std::cout << "@@@: " << node->name << " has " << node_p->mNumChildren << " children, and " << node_p->mNumMeshes << " Mesh instances" << std::endl;

    // assimp stores the transformations in
    std::memcpy(&node->transform[0][0], &node_p->mTransformation.a1,
                16 * sizeof(ai_real));
    node->transform = glm::transpose(node->transform);

    // loops through all mesh instances of this node
    node->meshInstances.reserve(node_p->mNumMeshes);
    for (int i = 0U; i < node_p->mNumMeshes; ++i) {
      auto meshInstance = std::make_shared<MeshInstance>();

      auto mesh = scene.m_meshes[node_p->mMeshes[i]];
      std::cout << "\tM@@@: " << mesh->name << std::endl;
      meshInstance->mesh = mesh;
      meshInstance->name = std::string{node_p->mName.C_Str()};
      meshInstance->material = mesh->material;
      meshInstance->transform = glm::mat4(1.0F);

      node->meshInstances.push_back(meshInstance);
    }

    // add the children for the next round
    for (int i = 0; i < node_p->mNumChildren; ++i) {
      queue.push_back(std::make_pair<>(node_p->mChildren[i], node));
    }
  }
}
void readLights() {
}

void readTextures(const aiScene *scene_p, Scene &scene) {
  for (int i=0; i<scene_p->mNumTextures; ++i) {
    auto aiTexture = scene_p->mTextures[i];
    std::cerr << "Could not load texture: " << aiTexture->mFilename.C_Str() << ": not yet implemented\n";
  }

}
void readCameras() {}

std::shared_ptr<void> SceneLoader::loadResource(const std::string &path) {
  geo::Scene scene;
  Assimp::Importer m_importer;
  const aiScene *aiScene_p{nullptr};

  aiScene_p = m_importer.ReadFile(path, aiProcess_Triangulate);
  if (!aiScene_p) {
    throw std::runtime_error(
        fmt::format("Failed to load scene file: {}", path));
  }

  readMaterials(aiScene_p, scene);
  readTextures(aiScene_p, scene);

  readMeshes(aiScene_p, scene);
  readHierarchy(aiScene_p, scene);

  return std::make_shared<Scene>(std::move(scene));
}
