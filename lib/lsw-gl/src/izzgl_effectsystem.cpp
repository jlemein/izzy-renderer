//
// Created by jeffrey on 17-12-21.
//
#include <geo_material.h>
#include <izzgl_effectsystem.h>
#include <izzgl_materialsystem.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using namespace izz;
using namespace izz::gl;

EffectSystem::EffectSystem(MaterialSystem& materialSystem)
  : m_materialSystem{materialSystem} {}

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

bool is_positive_number(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}
}  // namespace

void EffectSystem::readFramebuffers(const nlohmann::json& json) {
  for (const auto& framebuffer : json["framebuffers"]) {
    auto name = framebuffer["name"].get<std::string>();
    izz::geo::FramebufferConfiguration config;

    for (const auto& [attachment, value] : framebuffer["input"].items()) {
      auto bufferName = value.get<std::string>();
      if (!FramebufferPresets.contains(bufferName)) {
        throw std::runtime_error(fmt::format("Frambuffer '{}: attachment {}': specified buffer preset '{}' is unknown.", name, attachment, bufferName));
      }

      if (is_positive_number(attachment)) {
        int attachmentId = std::atoi(attachment.c_str());
        config.in_colorAttachments[attachmentId] = FramebufferPresets.at(bufferName);
      } else if (attachment == "DEPTH") {
        config.in_depthAttachment = FramebufferPresets.at(bufferName);
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
      effect.name = e.contains("name") ? e["name"].get<std::string>() : "''";

      spdlog::error("pass count: {}", e["passes"].size());

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

          auto node = geo::Node<std::shared_ptr<lsw::geo::Material>>();
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

            geo::Edge edge;
            edge.from = effect.graph.nodeIds[passId];
            auto targetPassId = bindInfo["target_pass"].is_number_integer() ? std::to_string(bindInfo["target_pass"].get<int>())
                                                                            : bindInfo["target_pass"].get<std::string>();
            edge.to = effect.graph.nodeIds[targetPassId];

            auto fromAttachment = std::atoi(fromAttachmentStr.c_str());
            auto dstAttachment = bindInfo["target_bind"].get<int>();
            edge.binding[fromAttachment] = dstAttachment;
            effect.graph.addEdge(edge);
          }
        } else {
          // default binding
          if (i + 1 < passCount) {
            const auto& nextPass = e["passes"][i + 1];
            auto nextPassId = std::to_string(i + 1);
            if (nextPass.contains("id")) {
              nextPassId = nextPass["id"].is_number_integer() ? std::to_string(nextPass["id"].get<int>()) : nextPass["id"].get<std::string>();
            }

            effect.graph.addNode(nextPassId);

            geo::Edge edge;
            edge.from = effect.graph.nodeIds.at(passId);
            edge.to = effect.graph.nodeIds.at(nextPassId);
            effect.graph.addEdge(edge);
          }
        }
      }
    }
  }
}