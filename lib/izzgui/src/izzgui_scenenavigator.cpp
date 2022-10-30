//
// Created by jeffrey on 12-12-21.
//
#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <ecs_camera.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <izz_relationshiputil.h>
#include <izzgl_entityfactory.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgui_imguix.h>
#include <izzgui_scenenavigator.h>
#include <spdlog/spdlog.h>
#include <izzgui_componenteditor.h>
using namespace izz::gui;

const Icon* SceneNavigator::getIconForEntity(const entt::registry& registry, entt::entity e) {
  static const Icon* DEFAULT_ICON = "";

  if (e == entt::null) {
    return ICON_FA_GLOBE;
  }

  if (registry.any_of<ecs::DirectionalLight>(e)) {
    return ICON_FA_SUN;
  }

  if (registry.any_of<ecs::AmbientLight, ecs::PointLight, ecs::SpotLight>(e)) {
    return ICON_FA_LIGHTBULB;
  }

  if (registry.any_of<ecs::Camera>(e)) {
    return ICON_FA_CAMERA;
  }

  auto relationship = registry.try_get<Relationship>(e);
  if (relationship && relationship->hasChildren()) {
    return ICON_FA_VECTOR_SQUARE;
  }

  return DEFAULT_ICON;
}

ImRect SceneNavigator::renderHierarchy(entt::registry& reg, entt::entity e, int depth) {
  std::string name = "World";
  auto icon = getIconForEntity(reg, e);
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;

  if (e != entt::null) {
    if (auto pName = reg.try_get<Name>(e)) {
      name = pName->name;
    }

    // display as leaf node, when Relationship does not exist, OR if the node does not contain children.
    if (auto pRelationship = reg.try_get<Relationship>(e); !pRelationship || !pRelationship->hasChildren()) {
      flags |= ImGuiTreeNodeFlags_Leaf;
    }
  }

  if (m_selectedHierarchyNode == e) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  std::string treeNodeText = fmt::format("{}  {}", icon, name);
  bool isClicked{false};
  bool recurse = ImGuiX::TreeNode(treeNodeText.c_str(), flags, depth, isClicked);

  if (isClicked) {
    m_selectedHierarchyNode = e;
    izz::gui::ComponentEditor::SelectedEntity = e;
  }

  if (recurse) {
    // add children (if they exist).
    if (e != entt::null) {
      if (auto pRel = reg.try_get<Relationship>(e); pRel && pRel->hasChildren()) {
        for (auto child : pRel->children) {
          renderHierarchy(reg, child, depth + 1);
        }
      }
    } else {
      auto relationships = m_sceneGraph->getRegistry().view<Relationship>();
      for (auto e : relationships) {
        if (!m_sceneGraph->getRegistry().get<Relationship>(e).hasParent()) {
          renderHierarchy(reg, e, depth + 1);
        }
      }
    }
  }

  ImRect nodeRect;
  return nodeRect;
}

SceneNavigator::SceneNavigator(std::shared_ptr<izz::EntityFactory> sceneGraph)
  : m_sceneGraph{sceneGraph} {}

void SceneNavigator::init() {
}

void SceneNavigator::render(float time, float dt) {
  if (SceneNavigator::Show) {
    if (ImGui::Begin(SceneNavigator::ID, &SceneNavigator::Show)) {
      renderHierarchy(m_sceneGraph->getRegistry(), entt::null);
    }
    ImGui::End();
  }
}

void SceneNavigator::Open() {
  SceneNavigator::Show = true;
}

void SceneNavigator::Toggle() {
  SceneNavigator::Show = !SceneNavigator::Show;
}