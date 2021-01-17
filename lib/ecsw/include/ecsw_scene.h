//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECSW_SCENE_H
#define GLVIEWER_ECSW_SCENE_H

#include <ecsw_model.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace artifax {
namespace ecsw {

//class MeshList;
//class ModelList;
//class LightList;
//class CameraList;
//class MaterialList;

/**
 * @details
 * For now Scene wraps an assimp pointer.
 */
class Scene {
public:
  Scene(const std::string& path);
  ~Scene();

  const aiScene* get();

private:
  Assimp::Importer m_importer;
  const aiScene* m_aiScene_p {nullptr};

//  MeshList getMeshes();
//
//  ModelList getAllModels();
//  ModelList getStaticModels();
//  ModelList getDynamicModels();
//
//  LightList getActiveLights();
//  LightList getLights();
//
//  CameraList getActiveCameras();
//  CameraList getCameras();
//
//  MaterialList getMaterials();
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSW_SCENE_H
