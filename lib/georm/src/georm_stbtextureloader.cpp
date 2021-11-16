//
// Created by jlemein on 22-02-21.
//
#include <georm_stbtextureloader.h>
#include <geo_texture.h>

#include <wsp_workspace.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace lsw;
using namespace lsw::georm;

namespace {
const std::vector<std::string> SUPPORTED_EXTENSIONS = {".bmp", ".hdr", ".jpg", ".jpeg", ".png", ".exr"};
}  // namespace

geo::Texture StbTextureLoader::loadTexture(const std::filesystem::path& path) {
  int desiredChannels = 4;
  int width, height, channels;
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

  geo::Texture texture {.path = path, .width = width, .height = height, .channels = desiredChannels};

  // number of pixels * channels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture.data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
  stbi_image_free(pixelData);

  return texture;
}

geo::ExtensionList StbTextureLoader::getSupportedExtensions() const {
  return SUPPORTED_EXTENSIONS;
}