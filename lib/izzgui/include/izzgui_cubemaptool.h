//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <izzgui_iguiwindow.h>
#include <memory>
#include <izzgui_shadereditor.h>

namespace izz {
class EntityFactory;

namespace gl {
class MaterialSystem;
class TextureSystem;
class MeshSystem;
}  // namespace gl

namespace gui {

class CubeMapTool : public gui::IGuiWindow {
 public:
  CubeMapTool(std::shared_ptr<gl::MaterialSystem> materialSystem, std::shared_ptr<gl::TextureSystem> textureSystem,
              std::shared_ptr<gl::MeshSystem> meshSystem, std::shared_ptr<izz::EntityFactory> sceneGraph);

  static inline const char* ID{"Cube map tool"};

  void init() override;
  void render(float time, float dt) override;
  static void Open();
  static void Toggle();
  static inline bool Show{true};

 private:
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem{nullptr};
  std::shared_ptr<izz::gl::MeshSystem> m_meshSystem{nullptr};
  std::shared_ptr<izz::EntityFactory> m_sceneGraph{nullptr};

  VertexBufferId m_vbo {-1};
  MaterialId m_material {-1};
  GLuint m_fbo; // framebuffer object id to render the debug view of the cube map into.
  izz::gl::Texture* m_projectedCubeMap {nullptr}; // projected cubemap texture
  int m_width = 1024;
  int m_height = 1024;
};

}  // namespace gui
}  // namespace izz
