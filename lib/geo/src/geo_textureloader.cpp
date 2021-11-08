//
// Created by jlemein on 22-02-21.
//
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <res_resource.h>

#include <fmt/format.h>

#define STB_IMAGE_IMPLEMENTATION
#include <memory>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <wsp_workspace.h>
#include <boost/algorithm/string.hpp>

using namespace lsw;
using namespace lsw::geo;
using namespace std;

namespace {
std::vector<std::string> SUPPORTED_EXTENSIONS = {".bmp", ".hdr", ".jpg", ".jpeg", ".png"};
}
std::unique_ptr<res::IResource> TextureLoader::createResource(const std::string& name) {
  using wsp::R;

  auto path = std::filesystem::path(name);  // R(name);

  if (!std::filesystem::exists(path)) {
    throw std::runtime_error("Cannot load texture from file '{}': file does not exist");
  }

  auto extLowerCase = boost::algorithm::to_lower_copy(path.extension().string());
  if (std::find(SUPPORTED_EXTENSIONS.begin(), SUPPORTED_EXTENSIONS.end(), extLowerCase) == SUPPORTED_EXTENSIONS.end()) {
    auto message = fmt::format("{} extension not supported, when trying to load '{}'. Supported extensions: [{}]", extLowerCase, path.string(),
                               fmt::join(SUPPORTED_EXTENSIONS, ", "));
    throw std::runtime_error(message);
  }

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

  auto textureResource =
      make_unique<res::Resource<Texture>>(Texture{.path = path, .width = width, .height = height, .channels = desiredChannels});
  auto& texture = *textureResource;

  // number of pixels * channels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture->data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
  stbi_image_free(pixelData);

  spdlog::log(spdlog::level::info, "Loaded texture {}, {} x {}, {} channels", path.c_str(), width, height, desiredChannels);

  return textureResource;
}