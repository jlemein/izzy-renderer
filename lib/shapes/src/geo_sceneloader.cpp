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

    aiColor3D color (0.f,0.f,0.f);
    mat_p->Get(AI_MATKEY_COLOR_DIFFUSE,color);
    material->diffuse.r = color.r;
    material->diffuse.g = color.g;
    material->diffuse.b = color.b;

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

void SceneLoader::readInstances(const aiScene *scene_p, Scene &scene) {
  std::deque<aiNode *> queue;
  queue.push_back(scene_p->mRootNode);

  while (!queue.empty()) {
    auto node_p = queue.front();
    queue.pop_front();

    SceneNode newNode;
    newNode.name = node_p->mName.C_Str();

    std::memcpy(&newNode.transform[0][0], &node_p->mTransformation.a1,
                16 * sizeof(ai_real));

    // loops through all mesh instances of this node
    newNode.meshInstances.reserve(node_p->mNumMeshes);
    for (int i = 0U; i < node_p->mNumMeshes; ++i) {
      auto meshInstance = std::make_shared<MeshInstance>();
      meshInstance->mMesh = scene.m_meshes[node_p->mMeshes[i]];
      meshInstance->mMaterial = nullptr;
      newNode.meshInstances.push_back(meshInstance);

      // is it needed to explicitly store the mesh instances
      scene.m_instances.emplace_back(meshInstance);
    }

    //
    unsigned int childOffset = scene.m_nodeHierarchy.size() + queue.size();

    // add the children for the next round
    for (int i = 0; i < node_p->mNumChildren; ++i) {
      newNode.children.push_back(childOffset + i + 1);
      queue.push_back(node_p->mChildren[i]);
    }

    scene.m_nodeHierarchy.emplace_back(newNode);
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
  readInstances(aiScene_p, scene);

  return std::make_shared<Scene>(std::move(scene));
}
