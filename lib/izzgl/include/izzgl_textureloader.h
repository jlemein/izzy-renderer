//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace izz {
namespace gl {

struct Texture;

using ExtensionList = std::vector<std::string>;

/**
 * A texture loader loads a file from a given path, and returns the texture content.
 */
class TextureLoader {
 public:
  virtual ~TextureLoader() = default;

  /**
   * Loads a texture and returns the texture contents.
   * @param path Path to the file texture. This method should only be called with supported file extensions retrieved from \see getSupportedExtensions.
   * @return Texture content.
   */
  virtual Texture loadImage(const std::filesystem::path& path) = 0;

  virtual Texture loadTextureFromMemory(unsigned char* pData, int size) = 0;

  /**
   * @return a list of file extensions that this texture loaders supports.
   */
  virtual ExtensionList getSupportedExtensions() const = 0;
};

}  // namespace gl
}  // namespace izz
