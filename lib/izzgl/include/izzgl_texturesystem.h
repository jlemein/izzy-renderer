//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include "izz_texture.h"
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
 * @details
 * There are different types of methods to be called:
 *
 * load* methods (e.g. Texture, loadCubeMap, etc.)
 *      Allocates a texture buffer and fills image data into the buffer, so that it is available in the GPU.
 *
 * allocate* methods (e.g. allocateTexture, allocateCubeMap, etc.)
 *      Allocates a texture buffer, but does not fill data. Data needs to be specified by user after creation.
 *      useful when texture units are filled in by the user, or as a target destination for results to be used in subsequent render passes.
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
   * Loads and creates a floating point HDR texture, specified by path name.
   * @param path The file location of the HDR texture.
   * @return A pointer to the Hdr texture.
   */
  Texture* loadHdrTexture(const std::filesystem::path& path);

  /**
   * Allocates a cube map (on the GPU) and loads the textures by file path.
   * Order of paths are: positive_x, negative_x, negative_y, positive_y, positive_z, negative_z.
   * @param paths Vector of paths. Size should be equal to 6. For different sizes, use @see loadTextureAtlas.
   * @return A generic texture object, with data bound to the GPU.
   */
  Texture* loadCubeMap(const std::vector<std::filesystem::path> paths);

  /**
   * Creates a texture from raw image data, usually used to load embedded textures.
   * @param path Path name to the embedded texture. Sometimes embedded textures still have an internal name.
   *             This name will then not point to an actual file.
   * @param data Pointer to raw data containing image data. Image data may be compressed and uncompressed.
   * @param size Size of the image data (in bytes).
   * @param extensionHint (Optional) tag to find the appropriate file loader.
   * @return
   */
  Texture* loadEmbeddedTexture(const std::filesystem::path& path, unsigned char* data, int size, std::string extensionHint = "");

  /**
   * Retrieves the texture by id, previously obtained via loadTexture or loadCubeMap.
   * @param id Texture id uniquely describing the texture entity.
   * @return A pointer to the texture.
   *
   * @throws std::runtime_error if texture with specified id could not be found.
   */
  Texture* getTextureById(TextureId id);

  /**
   * Allocates a texture on the GPU with the specified width and height.
   * @param width           Width in pixels.
   * @param height          Height in pixels.
   * @param format          Internal storage format of the texture data.
   * @return A pointer to the created texture.
   */
  Texture* allocateTexture(int width, int height, TextureFormat format = TextureFormat::RGBA8);

  /**
   * Registers texture to texture system. Only use this if
   * @return
   */
  Texture* registerTexture(Texture& texture);

  /**
   * Similar to allocating a texture (\see allocateTexture), but in this case a cubemap is allocated.
   * A cubemap has 6 sides each defined by a texture.
   * @return
   */
  Texture* allocateCubeMap();

  Texture* allocateDepthTexture(int width, int height);

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

  /**
   * @returns all textures created via the texture system. This includes, normal file-based textures, internal textures (depth buffer, intermediate render
   * results for multi-pass rendering).
   */
  inline const std::unordered_map<TextureId, Texture>& getTextures() const {
    return m_textures;
  }

  /**
   * @returns all textures that are named. Named textures are usually identified by file name.
   * To return all textures (including internal textures, such as created depth buffers, etc) call @see getTextures.
   */
  inline const std::unordered_map<std::string, TextureId>& getNamedTextures() const {
    return m_cachedTextures;
  }

 private:
  /**
   * Loads image data but does not yet allocate the texture on the GPU.
   * @param path
   * @return
   * @throws std::out_of_range if texture could not be found.
   */
  Texture* loadImageData(const std::filesystem::path& path);

  Texture* loadHdrImageData(const std::filesystem::path& path);

  std::unordered_map<std::string, std::shared_ptr<TextureLoader>> m_textureLoaders;
  std::unordered_map<std::string, TextureId> m_cachedTextures;  /// maps file name to texture id, so that textures can reuse already loaded image data.
  std::unordered_map<TextureId, Texture> m_textures;            /// map of all created textures identified by texture id.
};

}  // namespace gl
}  // namespace izz