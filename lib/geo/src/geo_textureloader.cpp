//
// Created by jlemein on 22-02-21.
//
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <res_resource.h>

#include <fmt/format.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <memory>

using namespace affx;
using namespace affx::geo;
using namespace std;

std::unique_ptr<res::IResource>
TextureLoader::loadResource(const std::string &name) {
  int desiredChannels = 4;
  int width, height, channels;
  unsigned char *pixelData =
      stbi_load(name.c_str(), &width, &height, &channels, desiredChannels);

  if (pixelData == nullptr) {
    throw std::runtime_error(
        fmt::format("Cannot load texture from file {}. Are you sure the filename and extension are correct?", name));
  }

  if (width * height == 0) {
    stbi_image_free(pixelData);
    throw std::runtime_error(fmt::format(
        "Cannot load texture from file {}: width or height is 0", name));
  }

  auto textureResource =
      make_unique<res::Resource<Texture>>(123, Texture{.path = name.c_str(),
                                                       .width = width,
                                                       .height = height,
                                                       .channels = desiredChannels});
  auto &texture = *textureResource;

  // number of pixels * channels * 8 bit
  uint64_t sizeImageData = width * height * desiredChannels;
  texture->data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
  stbi_image_free(pixelData);

  spdlog::log(spdlog::level::info, "Loaded texture {}, {} x {}, {} channels", name, width, height, desiredChannels);

  return textureResource;
}