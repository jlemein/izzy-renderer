//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include "izzgl_texture.h"
#include "izzgl_textureloader.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace izz {
namespace gl {

struct Texture;

/**
 * The texture system manages the textures in the scene.
 * Textures stored on the GPU are managed by the render system. Loading textures
 * is best done via the texture system.
 * It is responsible for delegating to appropriate texture loader (i.e. RGBA vs floating point format).
 *
 * @details Note that there is no texture component or whatsoever.
 */
class TextureSystem {
 public:
  /**
   * Creates a texture with the specified name.
   * @param path Path of the texture file.
   * @return a texture object that is shared with the texture system.
   */
  Texture* loadTexture(const std::filesystem::path& path);

  /**
   * Sets a texture loader for the specified extension(s).
   * There can only be one texture loader per extension.
   * @param extension Extension that the texture loader can read.
   * @param textureLoader The texture loader responsible for loading the specified texture format.
   *                      Setting the extension to a nullptr unsets the texture loader.
   */
  void setTextureLoader(const std::string& extension, std::shared_ptr<TextureLoader> textureLoader);

  /**
   * @overload
   */
  void setTextureLoader(const ExtensionList& extension, std::shared_ptr<TextureLoader> textureLoader);

  /**
   * Removes all added texture loaders.
   */
  void clearTextureLoaders();

  const std::unordered_map<std::string, Texture>& getTextures() const;

 private:
  std::unordered_map<std::string, std::shared_ptr<TextureLoader>> m_textureLoaders;
  std::unordered_map<std::string, Texture> m_cachedTextures;
  std::unordered_map<TextureId, Texture> m_textures;
};

}  // namespace gl
}  // namespace izz