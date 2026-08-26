//
// Created by jeffrey on 10-11-22.
//

#pragma once

#include <imgui.h>

namespace izz {
namespace gui {

using Icon = char;

/**
 * Extension Gui components on top of ImGui.
 * Ideally I didn't want to extend ImGui with custom logic. Especially the tree node was not how I wanted it to be.
 * It still is not optimal. The code here just does the trick.
 */
struct ImGuiX {
  static const Icon* FOLDER_COLLAPSED;  // default is ICON_FA_ANGLE_RIGHT;
  static const Icon* FOLDER_EXPANDED;   // default is ICON_FA_ANGLE_DOWN;

  static void SetFolderCollapsedIcon(const Icon* icon);  /// icon to use for folders that are collapsed. Used in treenode or hierarchy views.
  static void SetFolderExpandedIcon(const Icon* icon);   /// icon to use for folders that are expanded. Used in treenode or hierarchy views.

  static bool TreeNode(const char* label, ImGuiTreeNodeFlags flags, bool& isClicked);
  static bool TreeNode(const char* label, ImGuiTreeNodeFlags flags, int depth, bool& isClicked);
};

}  // namespace gui
}  // namespace izz
