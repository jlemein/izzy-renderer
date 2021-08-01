//
// Created by jlemein on 06-11-20.
//

#include <ecs_texturesystem.h>
#include <ecs_renderable.h>
#include <ecs_texture.h>
#include <ecsg_scenegraph.h>
#include <geo_material.h>
#include <geo_texture.h>

#include <GL/glew.h>

#include <iostream>
#include <spdlog/spdlog.h>

using namespace lsw::ecs;
using namespace lsw;

TextureSystem::TextureSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph)
: m_sceneGraph(sceneGraph)
{
}

//void TextureSystem::initialize() {
//  auto& registry = m_sceneGraph.getRegistry();
//  auto view = registry.view<Texture>();
//
//  for (auto entity : view) {
//    auto &texture = view.get<Texture>(entity);
//
//    setupTextures(texture);
//  }
//
//  //  auto pTexture = reinterpret_cast<Texture*>(mesh.getComponent("Texture"));
//}
namespace {
/**!
 * @brief Creates a texture buffer and sends the data over to the GPU. This
 * method therefore converts a texture resource to GPU ready representation.
 * @param geoTexture
 * @param name
 * @return
 */
ecs::Texture makeGlTexture(const geo::Texture &geoTexture,
                           const std::string &name) {
  ecs::Texture texture{.name = name};
  glGenTextures(1, &texture.glTextureId);
  glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!geoTexture.data.empty()) {
    GLint texChannel = geoTexture.channels == 3 ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, geoTexture.width, geoTexture.height,
                 0, texChannel, GL_UNSIGNED_BYTE, geoTexture.data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
    exit(1);
  }

  spdlog::log(spdlog::level::debug, "Loaded GL texture for texture '{}'", name);
  return texture;
}
}


void TextureSystem::initialize() {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<geo::Material>();

  for (auto entity : view) {
    auto& renderable = registry.get_or_emplace<Renderable>(entity);
    auto &geoMaterial = view.get<geo::Material>(entity);

    for (auto& [name, path] : geoMaterial.texturePaths) {
      // TODO: check if name is used in shader object

      renderable.textures.emplace_back(makeGlTexture(**geoMaterial.textures[name], name));
    }

    // A material is parsed in two ways
    // - A material has standard textures assigned in the scene file, such as
    //   diffuse, normal, transparency maps. They map to predefined variable names.
    // - A material requires a specific texture, such as noise, or any other texture of specific use for this material only.
    //   In that case,
    // diffuse texture
    if (geoMaterial.diffuseTexture != nullptr) {
      renderable.textures.emplace_back(makeGlTexture(**geoMaterial.diffuseTexture, "diffuseTex"));
    }
    if (geoMaterial.specularTexture != nullptr) {
      renderable.textures.emplace_back(makeGlTexture(**geoMaterial.specularTexture, "specularTex"));
    }
    if (geoMaterial.normalTexture != nullptr) {
      renderable.textures.emplace_back(makeGlTexture(**geoMaterial.normalTexture, "normalTex"));
    }
    if (geoMaterial.roughnessTexture != nullptr) {
      renderable.textures.emplace_back(makeGlTexture(**geoMaterial.roughnessTexture, "roughTex"));
    }
  }
}

void TextureSystem::update(float time, float dt) {}

void TextureSystem::onRender(entt::entity e) {
  //  for (auto entity : view) {
  auto& registry = m_sceneGraph->getRegistry();

  if (!registry.all_of<Renderable, geo::Material>(e)) {
    return;
  }

  const auto &renderable = registry.get<Renderable>(e);
//  const auto& shader = registry.get<Shader>(e);

  for (int t=0; t<renderable.textures.size(); ++t) {
    auto& texture = renderable.textures[t];

    glActiveTexture(GL_TEXTURE0+t);
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

    GLint s_diffuseMap = glGetUniformLocation(renderable.program, texture.name.c_str());
    //    if (s_diffuseMap == -1) {
    //        //std::cout << "Cannot find diffuse texture" << std::endl;
    //        //exit(1);
    //    } else {
    glUniform1i(s_diffuseMap, t);
  }


  //    }
  //    glBindSampler(0, 0);
}