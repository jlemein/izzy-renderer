//
// Created by jeffrey on 12-12-21.
//
#include <imgui.h>
#include <izzgl_entityfactory.h>
#include <izzgl_materialsystem.h>
#include <izzgl_meshsystem.h>
#include <izzgl_texturesystem.h>
#include <izzgui_cubemaptool.h>
#include <spdlog/spdlog.h>
using namespace izz::gui;

CubeMapTool::CubeMapTool(std::shared_ptr<gl::MaterialSystem> materialSystem, std::shared_ptr<gl::TextureSystem> textureSystem,
                         std::shared_ptr<gl::MeshSystem> meshSystem, std::shared_ptr<izz::EntityFactory> sceneGraph)
  : m_materialSystem{materialSystem}
  , m_textureSystem{textureSystem}
  , m_meshSystem{meshSystem}
  , m_sceneGraph{sceneGraph} {}

void CubeMapTool::init() {
  static float vertices[] = {
      -1.0, 0.33,  -0.5, -0.33, -.5, 0.33, -1.0, 0.33,  -1.0, -0.33, -.5, -0.33,  // left
      -.50, 0.33,  .0,   -0.33, .0,  0.33, -.50, 0.33,  -.50, -0.33, .0,  -0.33,  // front
      .0,   0.33,  .50,  -0.33, .50, 0.33, .0,   0.33,  .0,   -0.33, .50, -0.33,  // right
      .50,  0.33,  1.0,  -0.33, 1.0, 0.33, .50,  0.33,  .50,  -0.33, 1.0, -0.33,  // back
      -.50, -0.33, .0,   -1.0,  .0,  -.33, -.50, -0.33, -.50, -1.0,  .0,  -1.0,   // top
      -.50, 1.0,   .0,   0.33,  .0,  1.0,  -.50, 1.0,   -.50, 0.33,  .0,  0.33,   // bottom

  };

  // not used but required because of flaw in meshBuffer->bindBuffer (related to vertex attrib array enabling).
  static float normals[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  static float uv[] = {
      -1.0, 1.0, -1.0,   -1.0, -1.0, 1.0,    -1.0, 1.0, 1.0,    -1.0, 1.0, -1.0,    -1.0, -1.0, -1.0,    -1.0, -1.0, 1.0,  // left
      -1.0, 1.0, 1.0,     1.0, -1.0, 1.0,     1.0, 1.0, 1.0,     -1.0, 1.0, 1.0,     -1.0, -1.0, 1.0,     1.0, -1.0, 1.0,  // front
      1.0, 1.0, 1.0,    1.0, -1.0, -1.0,      1.0, 1.0, -1.0,     1.0, 1.0, 1.0,     1.0, -1.0, 1.0,     1.0, -1.0, -1.0,  // right

      1.0, 1.0, -1.0,     -1.0, -1.0, -1.0,     -1.0, 1.0, -1.0,     1.0, 1.0, -1.0,     1.0, -1.0, -1.0,     -1.0, -1.0, -1.0,  // back

      -1.0, -1.0, 1.0,    1.0, -1.0, -1.0,    1.0, -1.0, 1.0,    -1.0, -1.0, 1.0,    -1.0, -1.0, -1.0,    1.0, -1.0, -1.0,  // top
      -1.0, 1.0, -1.0,    1.0, 1.0, 1.0,    1.0, 1.0, -1.0,    -1.0, 1.0, -1.0,    -1.0, 1.0, 1.0,    1.0, 1.0, 1.0,  // bottom

//      .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0, .0

  };  // bottom

  izz::geo::Mesh mesh;
  mesh.vertices = std::vector<float>(vertices, vertices + sizeof(vertices) / sizeof(float));
  mesh.normals = std::vector<float>(normals, normals + sizeof(normals) / sizeof(float));
  mesh.uvs = std::vector<float>(uv, uv + sizeof(uv) / sizeof(float));
  mesh.numVertexCoords = 2U;
  mesh.numUvCoords = 3U;
  m_vbo = m_meshSystem->createVertexBuffer(mesh).id;

  m_material = m_materialSystem->createMaterial("RenderCubeMap").id;

  // create framebuffer object, that is needed to render projected cube map into.
  //  auto builder = m_renderSystem->frameBufferBuilder();
  //  auto buffer = builder.setDepthBuffer(200, 200).setColorBuffer().create();
  //  buffer.isValid();
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  m_projectedCubeMap = m_textureSystem->allocateTexture(m_width, m_height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_projectedCubeMap->bufferId, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created framebuffer for cube map tool.");
  } else {
    throw std::runtime_error("Failed creating a valid framebuffer for cube map tool.");
  }
}

void CubeMapTool::render(float time, float dt) {
  // render cubemap to separate texture.
  glViewport(0, 0, 1024, 768);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  m_meshSystem->bindBuffer(m_vbo);
  m_materialSystem->bindMaterial(m_material);

  const auto& meshBuffer = m_meshSystem->getMeshBuffer(m_vbo);
  glClearColor(.0F, .0F, .0F, 0.F);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(meshBuffer.primitiveType, 0, meshBuffer.drawElementCount);
  glEnable(GL_DEPTH_TEST);

  if (CubeMapTool::Show) {
    if (ImGui::Begin(CubeMapTool::ID, &CubeMapTool::Show)) {
      static TextureId currentItem = -1;
      izz::gl::Texture* pCurrent = nullptr;
      if (currentItem != -1) {
        pCurrent = m_textureSystem->getTextureById(currentItem);
        //        m_materialSystem->getMaterialById(m_material).setTexture("cubeMap", pCurrent);
      }

      const char* name = pCurrent ? pCurrent->name.c_str() : "None";

      if (ImGui::BeginCombo("Cube map", name)) {
        for (auto& [name, texture] : m_textureSystem->getTextures()) {
          spdlog::info("Texture name: {}", texture.name);
          if (texture.type == izz::gl::TextureType::CUBEMAP) {
            bool isSelected = currentItem == texture.id;
            if (ImGui::Selectable(texture.name.c_str(), isSelected)) {
              currentItem = texture.id;
            }
          }
        }
        ImGui::EndCombo();
      }

      if (pCurrent != nullptr) {
        auto width = ImGui::GetWindowContentRegionWidth();
        ImGui::Image((void*)(intptr_t)m_projectedCubeMap->bufferId, ImVec2(width, 3*width/4));
      }


    }

    ImGui::End();
  }
}

void CubeMapTool::Open() {
  CubeMapTool::Show = true;
}

void CubeMapTool::Toggle() {
  CubeMapTool::Show = !CubeMapTool::Show;
}