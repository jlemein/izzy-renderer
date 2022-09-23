//
// Created by jeffrey on 24-9-22.
//
#pragma once

#include <izzgl_scenedependentuniform.h>
#include <entt/entt.hpp>
#include "ecs_camera.h"
#include "ecs_name.h"
#include "ecs_transform.h"
#include "izzgl_material.h"
#include "izzgl_materialsystem.h"
#include "uniform_forwardlighting.h"
#include "uniform_mvp.h"

namespace izz::gl {

// struct MvpUtil {
//   static void UpdateRenderableMvps(MaterialSystem& materialSystem, const entt::registry& registry, entt::entity camera) {
//     if (camera == entt::null) {
//       throw std::runtime_error("No camera specified. Cannot update MVP's.");
//     }
//
//     auto& cameraTransform = registry.get<izz::ecs::Transform>(camera);
//     glm::mat4 view = glm::inverse(cameraTransform.worldTransform);
//
//     auto& activeCamera = registry.get<izz::ecs::Camera>(camera);
//     glm::mat4 proj = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
//
//     for (auto [e, r] : registry.view<Renderable>().each()) {
//       auto transform = registry.try_get<izz::ecs::Transform>(e);
//       auto model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);
//
//       try {
//         auto& material = materialSystem.getMaterialById(r.materialId);
//         ModelViewProjection* mvp = reinterpret_cast<ModelViewProjection*>(material.uniformBuffers.at("ModelViewProjection").data);
//         mvp->model = model;
//         mvp->view = view;
//         mvp->proj = proj;
//
//         // camera position is stored in 4-th column of inverse view matrix.
//         mvp->viewPos = glm::inverse(view)[3];
//
//       } catch (std::out_of_range&) {
//         auto materialName = materialSystem.getMaterialById(r.materialId).getName();
//         auto nameComponent = registry.try_get<izz::ecs::Name>(e);
//         auto name = (nameComponent != nullptr) ? nameComponent->name : "<unnamed>";
//
//         auto matTemplate = materialSystem.getMaterialTemplateFromMaterial(r.materialId);
//
//         auto basicMessage =
//             fmt::format("e:{} ({}): cannot access ModelViewProjection matrix for material '{}'. Does shader have 'ModelViewProjection' uniform buffer?",
//             e,
//                         name, materialName);
//         auto detailedMessage = fmt::format("Details:\n\tMaterial template: {},\n\tvertex shader: {},\n\tfragment shader: {}", matTemplate.name,
//                                            matTemplate.vertexShader, matTemplate.fragmentShader);
//         throw std::runtime_error(fmt::format("{}\n{}", basicMessage, detailedMessage));
//       } catch (std::runtime_error error) {
//         auto nameComponent = registry.try_get<izz::ecs::Name>(e);
//         auto name = (nameComponent != nullptr) ? nameComponent->name : "<unnamed>";
//         throw std::runtime_error(fmt::format("e:{} ({}): {}", e, name, error.what()));
//       }
//     }
//   }
// };

//struct MvpShared {
//  glm::mat4 viewMatrix;
//  glm::mat4 projectionMatrix;
//};




}  // namespace izz::gl
