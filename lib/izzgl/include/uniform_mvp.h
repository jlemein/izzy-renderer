#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct ModelViewProjection {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec3 viewPos;

  static inline const char* PARAM_NAME = "ModelViewProjection";
};

class MvpManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(ModelViewProjection);
    return new ModelViewProjection;
  }

  void printMatrix(std::string name, glm::mat4& data) {
    auto str = fmt::format("{}\n\t{} {} {} {}\n\t{} {} {} {}\n\t{} {} {} {}\n\t{} {} {} {}\n",
                name,
                data[0][0], data[0][1], data[0][2], data[0][3],
                data[1][0], data[1][1], data[1][2], data[1][3],
                data[2][0], data[2][1], data[2][2], data[2][3],
                data[3][0], data[3][1], data[3][2], data[3][3]);
    std::cout << str << std::endl;
  }

  void DestroyUniformBlock(void* data) override {
    auto mvp = reinterpret_cast<ModelViewProjection*>(data);
    delete mvp;
  }
  void UpdateUniform(void* data, const gl::Material& m) override {
    // do nothing
    auto p = reinterpret_cast<ModelViewProjection*>(data);

//    printMatrix("model", p->model);
//    printMatrix("view", p->view);
//    printMatrix("projection", p->projection);


//    spdlog::info("view: \n[{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}]", p->model[0][0], p->model[0][1], p->model[0][2], p->model[0][3],
//                 p->view[1][0], p->view[1][1], p->view[1][2], p->view[1][3],
//                 p->view[2][0], p->view[2][1], p->view[2][2], p->view[2][3],
//                 p->view[3][0], p->view[3][1], p->view[3][2], p->view[3][3]);
//    spdlog::info("Camera position: {} {} {}", p->viewPos[0], p->viewPos[1], p->viewPos[2]);
;  }
};



}
}  // namespace lsw