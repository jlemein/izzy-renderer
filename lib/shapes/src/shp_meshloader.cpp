//
// Created by jlemein on 07-11-20.
//
#include <shp_meshloader.h>

#include <shp_mesh.h>

#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace artifax::shp;

void MeshLoader::loadSceneFile(const std::string &name, shp::Mesh& mesh) {
  Assimp::Importer importer;
  const aiScene *pScene = importer.ReadFile(name, aiProcess_Triangulate);
  if (!pScene) {
    std::cout << "Failed to load scene file: " << name << std::endl;
    exit(1);
  }
  unsigned int embeddedTextureCount = pScene->mNumTextures;

  std::cout << " Embedded texture count: " << embeddedTextureCount << std::endl;
  for (int i = 0; i < embeddedTextureCount; ++i) {
    aiTexture *pTexture = pScene->mTextures[i];
    std::cout << pTexture->mFilename.C_Str() << std::endl;
  }

  auto numMeshes = pScene->mNumMeshes;
  std::cout << " Mesh count: " << numMeshes << std::endl;
  for (int i = 0; i < numMeshes; ++i) {
    auto pMesh = pScene->mMeshes[i];
//    mesh.name = pMesh->mName.C_Str();

    auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
    cout << "    - " << pMesh->mName.C_Str() << " has " << pMesh->mNumVertices
         << " vertices and " << pMesh->mNumFaces * 3 << " indices" << std::endl
         << "      - UV coordinates: "
         << (pMesh->HasTextureCoords(0) ? "Yes" : "No") << std::endl
         << "      - Material name: " << pMaterial->GetName().C_Str()
         << std::endl
         << "          Diffuse Texture count: "
         << pMaterial->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;

    for (int i = 0; i < pMesh->mNumVertices; ++i) {
      auto pVertex = pMesh->mVertices[i];
      mesh.vertices.push_back(pVertex.x);
      mesh.vertices.push_back(pVertex.y);
      mesh.vertices.push_back(pVertex.z);

      auto pNormal = pMesh->mNormals[i];
      mesh.normals.push_back(pNormal.x);
      mesh.normals.push_back(pNormal.y);
      mesh.normals.push_back(pNormal.z);

      aiVector3D ZERO3D = {0, 0, 0};
      auto uv =
          pMesh->HasTextureCoords(0) ? pMesh->mTextureCoords[0][i] : ZERO3D;
      mesh.uvs.push_back(uv.x);
      mesh.uvs.push_back(uv.y);
    }

    for (int f = 0; f < pMesh->mNumFaces; ++f) {
      auto face = pMesh->mFaces[f];
      mesh.indices.push_back(face.mIndices[0]);
      mesh.indices.push_back(face.mIndices[1]);
      mesh.indices.push_back(face.mIndices[2]);
    }
  }

  auto numMaterials = pScene->mNumMaterials;
  cout << " Material count: " << numMaterials << endl;
  for (int i = 0; i < numMaterials; ++i) {
    auto pMaterial = pScene->mMaterials[i];
    std::cout << "    - " << pMaterial->GetName().C_Str() << std::endl;
  }
  //
  //    mesh.vertices.resize(pScene->mMeshes[0]->mNumVertices);
  //    mesh.normals.resize(pScene->mMeshes[0]->mNumVertices);
  ////    mesh.uvs.resize(pScene->mMeshes[0]->mNumUVComponents[0]);
  //
  //    memcpy(mesh.vertices.data(), pScene->mMeshes[0]->mVertices,
  //    pScene->mMeshes[0]->mNumVertices * sizeof(float) * 3);
  //    memcpy(mesh.normals.data(), pScene->mMeshes[0]->mNormals,
  //    pScene->mMeshes[0]->mNumVertices * sizeof(float) * 3);
}

void MeshLoader::createSimpleTriangles(shp::Mesh &mesh) {
  std::vector<float> vertices = {-0.5, -0.5, 0.0, 0.5, -0.5,
                                 0.0,  0.0,  0.5, 0.0};
  mesh.vertices = vertices;
}