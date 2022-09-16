//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace izz {
namespace gl {

struct Texture;

using ExtensionList = std::vector<std::string>;

class TextureLoader {
 public:
  virtual ~TextureLoader() = default;

  virtual Texture loadTexture(const std::filesystem::path& path) = 0;

  virtual ExtensionList getSupportedExtensions() const = 0;
};

}  // namespace gl
}  // namespace izz
