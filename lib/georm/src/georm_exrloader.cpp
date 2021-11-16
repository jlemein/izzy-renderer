//
// Created by jeffrey on 15-11-21.
//
#include <geo_texture.h>
#include <georm_exrloader.h>
#include <algorithm>
#include <filesystem>

#include <ImfArray.h>
#include <ImfRgbaFile.h>

using namespace lsw;
using namespace lsw::georm;

using namespace Imf;
using namespace Imath;

namespace {
void transformHdrToLdr(const Array2D<Rgba>& src, geo::Texture& dst) {
  dst.data.resize(src.width() * src.height() * 4);
  uint8_t* pData = dst.data.data();
  dst.width = src.width();
  dst.height = src.height();
  dst.channels = 4U;
  dst.depth = 1;

  for (int y = 0; y < src.height(); ++y) {
    for (int x = 0; x < src.width(); ++x) {
      auto* pixel = reinterpret_cast<geo::Rgba8888*>(pData + (y * src.width() + x));
      pixel->r = static_cast<uint8_t>(std::clamp(src[y][x].r * 255.0F, .0F, 255.F));
      pixel->g = static_cast<uint8_t>(std::clamp(src[y][x].g * 255.0F, .0F, 255.F));
      pixel->b = static_cast<uint8_t>(std::clamp(src[y][x].b * 255.0F, .0F, 255.F));
      pixel->a = static_cast<uint8_t>(std::clamp(src[y][x].a * 255.0F, .0F, 255.F));
    }
  }
}
}  // namespace

geo::Texture ExrLoader::loadTexture(const std::filesystem::path& path) {
  // Read data via OpenEXR library into imageData
  RgbaInputFile file(path.c_str());
  Box2i dw = file.dataWindow();
  Array2D<Rgba> imageData;
  auto width = dw.max.x - dw.min.x + 1;
  auto height = dw.max.y - dw.min.y + 1;
  imageData.resizeErase(width, height);
  file.setFrameBuffer(&imageData[0][0] - dw.min.x - dw.min.y * width, 1, width);
  file.readPixels(dw.min.y, dw.max.y);

  // convert HDR to LDR to use as geo::Texture
  geo::Texture texture;
  transformHdrToLdr(imageData, texture);
  texture.path = path;

  return texture;
}

geo::ExtensionList ExrLoader::getSupportedExtensions() const {
  return geo::ExtensionList{"exr"};
}
