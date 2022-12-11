//
// Created by jlemein on 22-02-21.
//
#include "izzgl_stbtextureloader.h"
#include "izz_texture.h"

#include "wsp_workspace.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace izz::gl;

namespace {
const std::vector<std::string> SUPPORTED_EXTENSIONS = {".bmp", ".hdr", ".jpg", ".jpeg", ".png", ".exr"};
}  // namespace

StbTextureLoader::StbTextureLoader(bool flipVertical)
  : m_flipVertical{flipVertical} {}

Texture StbTextureLoader::loadImage(const std::filesystem::path& path) {
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

  Texture texture{.type = TextureType::TEXTURE_2D, .path = path, .width = width, .height = height, .numChannels = desiredChannels};

  // number of pixels * numChannels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture.data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
  stbi_image_free(pixelData);

  return texture;
}

Texture StbTextureLoader::loadHdrImage(const std::filesystem::path& path) {
  throw std::runtime_error(fmt::format("No support for loading HDR texture '{}'", path.string()));
}

Texture StbTextureLoader::loadTextureFromMemory(unsigned char* pData, int size) {
  int desiredChannels = 4;
  int width, height, channels;

  stbi_set_flip_vertically_on_load_thread(m_flipVertical);
  unsigned char* imageData = stbi_load_from_memory(pData, size, &width, &height, &channels, desiredChannels);

  Texture texture{.type = TextureType::TEXTURE_2D, .path = "", .width = width, .height = height, .numChannels = desiredChannels};

  // number of pixels * numChannels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture.data = std::vector<uint8_t>(imageData, imageData + sizeImageData);
  stbi_image_free(imageData);

  return texture;
}

void StbTextureLoader::writeTexture(Texture const* texture, std::filesystem::path path) {
  spdlog::info("Writing texture {}x{} to file {}", texture->width, texture->height, path.c_str());
  stbi_write_png(path.c_str(), texture->width, texture->height, texture->numChannels, texture->data.data(), 0);
}

ExtensionList StbTextureLoader::getSupportedExtensions() const {
  return SUPPORTED_EXTENSIONS;
}