//
// Created by jeffrey on 08-12-21.
//
#pragma once

#include <memory>

namespace lsw {

namespace ecsg {
class SceneGraph;
};

namespace glrs {

class IMaterialSystem;

class PostprocessingSystem {
 public:
  PostprocessingSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<glrs::IMaterialSystem> materialSystem);

  void initialize();

  void update();

 private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph {nullptr};
  std::shared_ptr<IMaterialSystem> m_materialSystem {nullptr};
};

}  // namespace glrs
}  // namespace lsw
