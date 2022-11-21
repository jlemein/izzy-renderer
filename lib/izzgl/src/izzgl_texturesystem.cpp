//
// Created by jeffrey on 15-11-21.
//
#include "izzgl_texturesystem.h"
#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>

using namespace izz::gl;

namespace {
/**!
 * @brief Creates a texture buffer and sends the data over to the GPU. This
 * method therefore converts a texture resource to GPU ready representation.
 * @param texture
 *
 * TODO: think about passing a scene graph entity instead
 * @return
 */
GLuint allocateTextureBuffer(const Texture& texture) {
  GLuint bufferId;
  glGenTextures(1, &bufferId);
  glBindTexture(GL_TEXTURE_2D, bufferId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture.data.empty()) {
    GLint colorFormat = texture.channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, texture.width, texture.height, 0, colorFormat, GL_UNSIGNED_BYTE, texture.data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    spdlog::error("Failed to create texture buffer for texture '{}'. Data is empty.", texture.path);
    exit(1);
  }

  return bufferId;
}

void uploadCubeMapData(const Texture textures[], int length) {
  for (int i = 0; i < length; ++i) {
    GLint colorFormat = textures[i].channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, colorFormat, textures[i].width, textures[i].height, 0, colorFormat, GL_UNSIGNED_BYTE,
                 textures[i].data.data());
  }
}
}  // namespace

Texture* TextureSystem::loadImageData(const std::filesystem::path& path) {
  auto extension = path.extension().string();
  boost::to_lower(extension);

  if (m_cachedTextures.count(path) > 0) {
    return &m_textures.at(m_cachedTextures.at(path.string()));
  }
  if (m_textureLoaders.count(extension) == 0) {
    spdlog::warn("Texture system cannot load texture '{}'. Unrecognized extension {}.", path.string(), extension);
    return nullptr;
  }

  spdlog::info("TextureSystem: loading texture '{}'", path.string());
  Texture texture = m_textureLoaders.at(extension)->loadImage(path);

  texture.id = m_textures.size() + 1;
  m_textures[texture.id] = texture;
  m_cachedTextures[path] = texture.id;

  return &m_textures[texture.id];
}

Texture* TextureSystem::loadTexture(const std::filesystem::path& path) {
  auto pTexture = loadImageData(path);
  pTexture->bufferId = allocateTextureBuffer(*pTexture);
  return pTexture;
}

Texture* TextureSystem::loadCubeMap(const std::vector<std::filesystem::path> paths) {
  if (paths.size() != 6) {
    throw std::runtime_error("Cannot load cube map: a cube map requires 6 textures.");
  }

  auto cubeMap = allocateCubeMap();
  cubeMap->path = paths[0].parent_path();

  for (int i = 0; i < paths.size(); ++i) {
    auto imageData = loadImageData(paths[i]);

    cubeMap->width = imageData->width;
    cubeMap->height = imageData->height;
    cubeMap->depth = imageData->depth;
    cubeMap->channels = imageData->channels;

    GLint colorFormat = cubeMap->channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, colorFormat, imageData->width, imageData->height, 0, colorFormat, GL_UNSIGNED_BYTE,
                 imageData->data.data());

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }

  return cubeMap;
}

Texture* TextureSystem::loadEmbeddedTexture(const std::filesystem::path& path, unsigned char* data, int size, std::string extensionHint) {
  auto extension = extensionHint;
  if (extensionHint.empty()) {
    extension = path.extension().string();
  }
  boost::to_lower(extension);

  if (m_textureLoaders.count(extension) == 0) {
    spdlog::warn("Texture system cannot load texture '{}'. Unrecognized extension {}.", path.string(), extension);
    return nullptr;
  }

  spdlog::info("TextureSystem: loading embedded texture '{}'", path.string());
  Texture texture = m_textureLoaders.at(extension)->loadTextureFromMemory(data, size);

  texture.id = m_textures.size() + 1;
  m_textures[texture.id] = texture;
  m_cachedTextures[path] = texture.id;

  auto pTexture = &m_textures[texture.id];
  pTexture->path = path;
  pTexture->bufferId = allocateTextureBuffer(*pTexture);
  return pTexture;
}

Texture* TextureSystem::getTextureById(TextureId id) {
  return &m_textures.at(id);
}

Texture* TextureSystem::allocateTexture(int width, int height) {
  Texture texture;
  texture.type = TextureType::TEXTURE_2D;

  glGenTextures(1, &texture.bufferId);
  glBindTexture(GL_TEXTURE_2D, texture.bufferId);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  texture.id = m_textures.size() + 1;
  m_textures[texture.id] = texture;
  return &m_textures[texture.id];
}

Texture* TextureSystem::allocateCubeMap() {
  Texture texture;
  texture.type = TextureType::CUBEMAP;

  glGenTextures(1, &texture.bufferId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture.bufferId);

  texture.id = m_textures.size() + 1;
  m_textures[texture.id] = texture;
  return &m_textures[texture.id];
}

Texture* TextureSystem::allocateDepthTexture(int width, int height) {
  Texture texture;
  texture.type = TextureType::TEXTURE_2D;

  glGenTextures(1, &texture.bufferId);
  glBindTexture(GL_TEXTURE_2D, texture.bufferId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

  texture.id = m_textures.size() + 1;
  m_textures[texture.id] = texture;
  return &m_textures[texture.id];
}

void TextureSystem::setTextureLoader(const std::string& extension, std::shared_ptr<TextureLoader> textureLoader) {
  if (extension.empty()) {
    throw std::runtime_error("Cannot set texture loader for empty extension.");
  }

  auto canonicalExtension = boost::starts_with(extension, ".") ? extension : "." + extension;
  boost::to_lower(canonicalExtension);

  m_textureLoaders[canonicalExtension] = textureLoader;
}

void TextureSystem::setTextureLoader(const std::vector<std::string>& extensions, std::shared_ptr<TextureLoader> textureLoader) {
  for (const auto& ext : extensions) {
    setTextureLoader(ext, textureLoader);
  }
}

void TextureSystem::clearTextureLoaders() {
  m_textureLoaders.clear();
}
