//
// Created by jeffrey on 17-12-21.
//
#include <geo_effect.h>
#include <geo_material.h>
#include <izz_scenegraph.h>
#include <izzgl_effectsystem.h>
#include <izzgl_materialsystem.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using namespace izz;
using namespace izz::gl;

EffectSystem::EffectSystem(izz::SceneGraph& sceneGraph, MaterialSystem& materialSystem)
  : m_sceneGraph{sceneGraph}
  , m_materialSystem{materialSystem} {}

void EffectSystem::readEffectsFromFile(const std::filesystem::path& path) {
  std::ifstream input(path);
  if (input.fail()) {
    throw std::runtime_error(fmt::format("EffectSystem fails to initialize: cannot read from '{}'", path.c_str()));
  }

  try {
    nlohmann::json json;
    input >> json;
    readFramebuffers(json);
    readEffectsFromJson(json);
  } catch (nlohmann::detail::exception& e) {
    input.close();
    throw std::runtime_error(fmt::format("Failed parsing effects from '{}': {}", path.c_str(), e.what()));
  }

  input.close();
}

namespace {
std::unordered_map<std::string, geo::Buffer> FramebufferPresets{
    {std::string{"HDR_COLORBUFFER_RGBA"}, geo::Buffer{.bufferType = geo::FramebufferBufferType::TEXTURE,
                                                      .dataType = geo::FramebufferDataType::HALF,
                                                      .channelFormat = geo::FramebufferChannelFormat::RGBA,
                                                      .multisamples = 0}},
    {std::string{"COLORBUFFER_RGBA"}, geo::Buffer{.bufferType = geo::FramebufferBufferType::TEXTURE,
                                                  .dataType = geo::FramebufferDataType::UNSIGNED_BYTE,
                                                  .channelFormat = geo::FramebufferChannelFormat::RGBA,
                                                  .multisamples = 0}},
    {std::string{"DEPTH"}, geo::Buffer{.bufferType = geo::FramebufferBufferType::RENDERBUFFER,
                                       .dataType = geo::FramebufferDataType::UNSIGNED_BYTE,
                                       .channelFormat = geo::FramebufferChannelFormat::DEPTH,
                                       .multisamples = 0}}

};

void ResolveChannelAndDataType(geo::FramebufferChannelFormat fbChannel, geo::FramebufferDataType fbDataType,
                               GLuint& internalFormat, GLuint& format, GLuint& type) {
  if (fbChannel == geo::FramebufferChannelFormat::RGBA && fbDataType == geo::FramebufferDataType::FLOAT32) {
    internalFormat = GL_RGBA32F;
    format = GL_RGBA;
    type = GL_FLOAT;
  } else if (fbChannel == geo::FramebufferChannelFormat::RGBA && fbDataType == geo::FramebufferDataType::HALF) {
    internalFormat = GL_RGBA16F;
    format = GL_RGBA;
    type = GL_HALF_FLOAT;
  } else if (fbChannel == geo::FramebufferChannelFormat::RGBA && fbDataType == geo::FramebufferDataType::UNSIGNED_BYTE) {
    internalFormat = GL_RGBA;
    format = GL_RGBA;
    type = GL_UNSIGNED_BYTE;
  } else if (fbChannel == geo::FramebufferChannelFormat::RGB && fbDataType == geo::FramebufferDataType::FLOAT32) {
    internalFormat = GL_RGB;
    format = GL_RGB;
    type = GL_FLOAT;
  } else if (fbChannel == geo::FramebufferChannelFormat::RGB && fbDataType == geo::FramebufferDataType::HALF) {
    internalFormat = GL_RGB;
    format = GL_RGB;
    type = GL_HALF_FLOAT;
  } else if (fbChannel == geo::FramebufferChannelFormat::RGB && fbDataType == geo::FramebufferDataType::UNSIGNED_BYTE) {
    internalFormat = GL_RGB;
    format = GL_RGB;
    type = GL_UNSIGNED_BYTE;
  } else {
    spdlog::warn("Could not resolve framebuffer channel and data type. Using default RGBA (unsigned byte).");
    internalFormat = GL_RGBA;
    format = GL_RGBA;
    type = GL_FLOAT;
  }
}

bool is_positive_number(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}
}  // namespace

geo::cEffect EffectSystem::createEffect(const std::string& name) {
  if (m_effects.count(name) <= 0) {
    throw std::runtime_error(fmt::format("Cannot find effect with name '{}'", name));
  }

  return geo::cEffect{name};
}

void EffectSystem::readFramebuffers(const nlohmann::json& json) {
  spdlog::info("--- READING FRAMEBUFFERS ---");
  for (const auto& framebuffer : json["framebuffers"]) {
    auto name = framebuffer["name"].get<std::string>();
    izz::geo::FramebufferConfiguration config;

//    for (const auto& [attachment, value] : framebuffer["input"].items()) {
//      auto bufferName = value.get<std::string>();
//      if (!FramebufferPresets.contains(bufferName)) {
//        throw std::runtime_error(fmt::format("Frambuffer '{}: attachment {}': specified buffer preset '{}' is unknown.", name, attachment, bufferName));
//      }
//
//      if (is_positive_number(attachment)) {
//        int attachmentId = std::atoi(attachment.c_str());
//        config.colorAttachments[attachmentId] = FramebufferPresets.at(bufferName);
//      } else if (attachment == "DEPTH") {
//        config.depthAttachment = FramebufferPresets.at(bufferName);
//      } else if (attachment == "STENCIL") {
//        spdlog::warn("Attachment \"STENCIL\" is unsupported for the moment");
//      } else if (attachment == "DEPTH_STENCIL") {
//        spdlog::warn("Attachment \"DEPTH_STENCIL\" is unsupported for the moment");
//      } else {
//        spdlog::warn("Unrecognized input attachment specified: '{}'. This will be ignored.", attachment);
//      }
//    }

    for (const auto& [attachment, value] : framebuffer["output"].items()) {
      auto bufferName = value.get<std::string>();
      if (!FramebufferPresets.contains(bufferName)) {
        throw std::runtime_error(fmt::format("Frambuffer '{}: attachment {}': specified buffer preset '{}' is unknown.", name, attachment, bufferName));
      }

      if (is_positive_number(attachment)) {
        int attachmentId = std::atoi(attachment.c_str());
        config.colorAttachments[attachmentId] = FramebufferPresets.at(bufferName);
      } else if (attachment == "DEPTH") {
        config.depthAttachment = FramebufferPresets.at(bufferName);
      } else if (attachment == "STENCIL") {
        spdlog::warn("Attachment \"STENCIL\" is unsupported for the moment");
      } else if (attachment == "DEPTH_STENCIL") {
        spdlog::warn("Attachment \"DEPTH_STENCIL\" is unsupported for the moment");
      } else {
        spdlog::warn("Unrecognized input attachment specified: '{}'. This will be ignored.", attachment);
      }
    }

    m_framebuffers[name] = config;
    spdlog::info("Framebuffer '{}' registered successfully", name);
  }
}

void EffectSystem::readEffectsFromJson(const nlohmann::json& json) {
  bool hasEffects = json.contains("effects");
  if (hasEffects) {
    for (const auto& e : json["effects"]) {
      geo::Effect effect;
      effect.name = (e.contains("name") ? e["name"].get<std::string>() : "''");

      const auto passCount = e["passes"].size();
      for (int i = 0; i < passCount; ++i) {
        const auto& pass = e["passes"][i];
        auto passId = std::to_string(i);
        if (pass.contains("id")) {
          passId = pass["id"].is_number_integer() ? std::to_string(pass["id"].get<int>()) : pass["id"].get<std::string>();
        }

        try {
          auto materialName = pass["material"];
          auto framebufferName = pass["framebuffer"];

          geo::EffectNode node;
          node.material = m_materialSystem.createMaterial(materialName);

          if (m_framebuffers.count(framebufferName) <= 0) {
            throw std::runtime_error(fmt::format("Framebuffer with name '{}' is unknown.", framebufferName));
          }
          node.framebuffer = m_framebuffers.at(framebufferName);
          effect.graph[passId] = node;
          spdlog::info("Effect {} has pass {} with frame buffer {}", effect.name, materialName, framebufferName);
        } catch (nlohmann::detail::exception e) {
          throw std::runtime_error(fmt::format("Failed processing pass #{}:{}: {}", i, passId, e.what()));
        }

        // parse the binding
        if (pass.contains("binding")) {
          for (const auto& [fromAttachmentStr, bindInfo] : pass["binding"].items()) {
            if (!bindInfo.contains("target_pass") || !bindInfo.contains("target_bind")) {
              throw std::runtime_error(
                  fmt::format("Processing pass {}:#{} (id: {}): should have 'target_pass' and 'target_bind' specified, but one or both are not found.",
                              effect.name, i, passId));
            }

            auto targetPassId = bindInfo["target_pass"].is_number_integer() ? std::to_string(bindInfo["target_pass"].get<int>())
                                                                            : bindInfo["target_pass"].get<std::string>();
            auto fromAttachment = std::atoi(fromAttachmentStr.c_str());
            auto dstAttachment = bindInfo["target_bind"].get<int>();

            geo::BufferMapping mapping;
//            mapping.buffers[fromAttachment] = dstAttachment;
            geo::BufferMap bmap;
            bmap.outLocation = dstAttachment;
            bmap.inTextureUnit = 0;
            bmap.textureBuffer = 0;
            mapping.bufferMappings.push_back(bmap);
            effect.graph.connect(passId, targetPassId, mapping);
          }
        } else {
          // default binding
          if (i + 1 < passCount) {
            const auto& nextPass = e["passes"][i + 1];
            auto nextPassId = std::to_string(i + 1);
            if (nextPass.contains("id")) {
              nextPassId = nextPass["id"].is_number_integer() ? std::to_string(nextPass["id"].get<int>()) : nextPass["id"].get<std::string>();
            }

            auto materialName = nextPass["material"];
            auto framebufferName = nextPass["framebuffer"];
            geo::EffectNode node;
            node.material = m_materialSystem.createMaterial(materialName);
            node.framebuffer = m_framebuffers.at(framebufferName);

            effect.graph[nextPassId] = node;
            effect.graph.connect(passId, nextPassId);
          }
        }
      }

      m_effects[effect.name] = std::move(effect);
    }
  }
}

void createBuffer(GLuint attachment, int width, int height, const geo::Buffer& buffer) {
  if (buffer.bufferType == geo::FramebufferBufferType::RENDERBUFFER) {
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
  }
  if (buffer.bufferType == geo::FramebufferBufferType::TEXTURE) {
    GLuint internalFormat, format, type;
    ResolveChannelAndDataType(buffer.channelFormat, buffer.dataType,
                              internalFormat, format, type);

    GLuint tbo;
    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tbo, 0);
  }
}

void EffectSystem::initialize() {
  // A framebu
  // Flow of framebuffer operations.
  // First pass:
  // * Setup texture units based on in_fb configuration.
  spdlog::info("Nothing to initialize");
  int width = 1024, height = 1024;

  for (const auto& [e, cfx] : m_sceneGraph.getRegistry().view<geo::cEffect>().each()) {
    auto& fx = m_effects.at(cfx.name);
    spdlog::warn("Effect {} has node size: {}", fx.name, fx.graph.nodeSize());

    for (int i = 0; i < fx.graph.nodeSize(); ++i) {
      const auto& fb = fx.graph.nodes()[i].framebuffer;
      const auto& node = fx.graph.nodes()[i];

      GLuint fbo;
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      if (fb.depthAttachment.bufferType != geo::FramebufferBufferType::UNUSED) {
        createBuffer(GL_DEPTH_ATTACHMENT, width, height, fb.depthAttachment);
      }
      if (fb.depthStencilAttachment.bufferType != geo::FramebufferBufferType::UNUSED) {
        createBuffer(GL_DEPTH_STENCIL_ATTACHMENT, width, height, fb.depthStencilAttachment);
      }
      if (fb.stencilAttachment.bufferType != geo::FramebufferBufferType::UNUSED) {
        createBuffer(GL_STENCIL_ATTACHMENT, width, height, fb.stencilAttachment);
      }
      for (int i = 0; i < 4; ++i) {
        if (fb.colorAttachments[i].bufferType != geo::FramebufferBufferType::UNUSED) {
          createBuffer(GL_COLOR_ATTACHMENT0 + i, width, height, fb.colorAttachments[i]);
        }
      }

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer invalid");
      } else {
        node.material->fbo = fbo;
      }


//      node.renderPass = m_sceneGraph.makeRenderable()
      // add a renderable (or rename to renderpass)
//      node.renderpass =
    }

    //
    //    for (auto materialId : effect) {
    //      int numIncomingConnections = effect.getIncomingConnections(materialId).size();
    //      int numOutgoingConnections = effect.getIncomingConnections(materialId).size();
    //      glGenFramebuffers()
    //    }
    //
    //    effect.graph.
    //    glGenFramebuffers();
  }
}

geo::Effect& gl::EffectSystem::getEffect(geo::cEffect cEffect) {
  return m_effects.at(cEffect.name);
}

const geo::Effect& gl::EffectSystem::getEffect(geo::cEffect cEffect) const {
  return m_effects.at(cEffect.name);
}