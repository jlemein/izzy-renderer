//
// Created by jlemein on 22-02-21.
//
#include "izzgl_stbtextureloader.h"
#include "izzgl_texture.h"

#include "wsp_workspace.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace izz::gl;

namespace {
const std::vector<std::string> SUPPORTED_EXTENSIONS = {".bmp", ".hdr", ".jpg", ".jpeg", ".png", ".exr"};
}  // namespace

StbTextureLoader::StbTextureLoader(bool flipVertical)
  : m_flipVertical{flipVertical}
  {}

Texture StbTextureLoader::loadTexture(const std::filesystem::path& path) {
  int desiredChannels = 4;
  int width, height, channels;

  stbi_set_flip_vertically_on_load_thread(m_flipVertical);
  unsigned char* pixelData = stbi_load(path.c_str(), &width, &height, &channels, desiredChannels);

  if (pixelData == nullptr) {
    throw std::runtime_error(
        fmt::format("Cannot load texture from file '{}'. Are you sure the "
                    "filename and extension are correct?",
                    path.c_str()));
  }

  if (width * height == 0) {
    stbi_image_free(pixelData);
    throw std::runtime_error(fmt::format("Cannot load texture from file '{}': width or height is 0", path.c_str()));
  }

  Texture texture{.path = path, .width = width, .height = height, .channels = desiredChannels};

  // number of pixels * channels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture.data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
  stbi_image_free(pixelData);

  return texture;
}

Texture StbTextureLoader::loadTextureFromMemory(unsigned char* pData, int size) {
  int desiredChannels = 4;
  int width, height, channels;

  stbi_set_flip_vertically_on_load_thread(m_flipVertical);
  unsigned char* imageData = stbi_load_from_memory(pData, size, &width, &height, &channels, desiredChannels);

  Texture texture {.path = "", .width = width, .height = height, .channels = desiredChannels};

  // number of pixels * channels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture.data = std::vector<uint8_t>(imageData, imageData + sizeImageData);
  stbi_image_free(imageData);

  return texture;
}

ExtensionList StbTextureLoader::getSupportedExtensions() const {
  return SUPPORTED_EXTENSIONS;
}