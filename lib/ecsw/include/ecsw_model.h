//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECSW_MODEL_H
#define GLVIEWER_ECSW_MODEL_H

#include <ecsw_material.h>
#include <ecsw_meshinstance.h>
#include <ecsw_transform.h>

#include <memory>
#include <vector>

namespace izz {
namespace ecsw {

class Model_Node {
  std::shared_ptr<Model_Node> parent {nullptr};
  std::vector<Model_Node> m_children {};
  std::unique_ptr<MeshInstance> m_meshInstance {nullptr};
  Transform m_transform;
};

class Model {
public:
  typedef Model_Node Node;

  std::shared_ptr<Model::Node> getRootNode();
  std::vector<Model::Node> getNodes();

private:
  std::vector<Model_Node> m_hierarchy;
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSW_MODEL_H
