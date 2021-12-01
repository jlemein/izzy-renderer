//
// Created by jeffrey on 15-11-21.
//
#include <izz_texturesystem.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>

using namespace lsw;

std::shared_ptr<geo::Texture> TextureSystem::loadTexture(const std::filesystem::path& path) {
  auto extension = path.extension().string();
  boost::to_lower(extension);

  if (m_cachedTextures.count(path) > 0) {
    auto texture = std::make_shared<geo::Texture>(m_cachedTextures.at(path.string()));
    return texture;
  }
  if (m_textureLoaders.count(extension) == 0) {
    spdlog::warn("Texture system cannot load texture '{}'. Unrecognized extension {}.", path.string(), extension);
    return nullptr;
  }

  m_cachedTextures[path] = m_textureLoaders.at(extension)->loadTexture(path);
  return std::make_shared<geo::Texture>(m_cachedTextures[path]);
}

void TextureSystem::setTextureLoader(const std::string& extension, std::shared_ptr<geo::TextureLoader> textureLoader) {
  if (extension.empty()) {
    throw std::runtime_error("Cannot set texture loader for empty extension.");
  }

  auto canonicalExtension = boost::starts_with(extension, ".") ? extension : "." + extension;
  boost::to_lower(canonicalExtension);

  m_textureLoaders[canonicalExtension] = textureLoader;
}

void TextureSystem::setTextureLoader(const std::vector<std::string>& extensions, std::shared_ptr<geo::TextureLoader> textureLoader) {
  for (const auto& ext : extensions) {
    setTextureLoader(ext, textureLoader);
  }
}

void TextureSystem::clearTextureLoaders() {
  m_textureLoaders.clear();
}

const std::unordered_map<std::string, geo::Texture>& TextureSystem::getTextures() const {
  return m_cachedTextures;
}