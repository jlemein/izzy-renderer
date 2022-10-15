//
// Created by jeffrey on 14-10-22.
//
#pragma once

#include <geo_mesh.h>
#include <izzgl_material.h>
#include <entt/fwd.hpp>

namespace izz {
namespace gl {

struct ForwardRenderable;
class MaterialSystem;
class TextureSystem;
class MeshSystem;

/**
 * Depth peeling is an alpha blending strategy that can only be used in forward rendering mode.
 */
class DepthPeeling {
 public:
  /**
   * Depth peeling constructor.
   * @param materialSystem  Material system for accessing the materials during rendering.
   * @param meshSystem      Mesh system to
   * @param registry
   * @param numPeelPasses Number of peeling passes to use. Larger values negatively impact performance.
   *                  An amount of 3 means, you can see through 3 layers of transparent surfaces.
   */
  DepthPeeling(std::shared_ptr<MaterialSystem> materialSystem, std::shared_ptr<TextureSystem> textureSystem, std::shared_ptr<MeshSystem> meshSystem,
               const entt::registry& registry, int numPeelPasses = 3);

  /**
   * Intialize the framebuffers and textures used for depth peeling to the specified width. Subsequent framebuffer size changes
   * should be communicated via the method \see resize(width, height).
   * @param width Width of the framebuffer.
   * @param height Height of the framebuffer.
   */
  void init(int width, int height);

  /**
   * Resizes the framebuffers and textures.
   * @param width
   * @param height
   */
  void resize(int width, int height) {
    throw std::runtime_error("Not yet implemented");
  }

  /**
   * Renders transparent objects using depth peeling.
   * This method should be called after all opaque objects have been rendered to default framebuffer 0
   * with the color and depth buffer enabled.
   */
  void render();

 public:
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem{nullptr};
  std::shared_ptr<izz::gl::MeshSystem> m_meshSystem{nullptr};
  const entt::registry& m_registry;
  int m_numPeelPasses;

  MaterialId m_renderTextureMaterialId;    /// Material id for rendering onto screen space quad.
  MeshBufferId m_screenSpaceMeshBufferId;  /// Vertex buffer defining the screen space quad.
  int m_width;                             /// Width of the framebuffer (and accompanying texture buffers).
  int m_height;                            /// Height of the framebuffer (and accompanying texture buffers).
  GLuint m_opaqueFbo;                      /// Framebuffer object to render opaque render result into.
  Texture* m_opaqueTexture;                /// Color map of opaque objects.
  Texture* m_depthTexture;                 /// Depth map of opaque objects.
  GLuint m_peelFbo;                        /// Framebuffer Object where peeled render results are rendered to.
  GLuint m_peelColor;                      /// Color map of transparent objects.
  GLuint m_peelDepth;                      /// Depth map for the peel render pass.
  GLuint m_peelPrevColor;                  /// Previous color map, used to retrieve alpha value from previous pass.
  GLuint m_peelPrevDepth;                  /// Depth map of the previous peel pass.

  void createScreenSpaceRect();
  void renderTransparentObjects(bool isDepthPeeling);
};

}  // namespace gl
}  // namespace izz
