#pragma once

#include <string>
#include <filesystem>
#include <memory>

namespace izz {
namespace wsp {

/**
 * @brief The workspace parses the program arguments. *
 */
struct Workspace {

  /// @brief Scene file to be loaded, as specified on the command line
  std::filesystem::path sceneFile{""};

  /// @brief Materials file as specified by command line arguments.
  std::filesystem::path materialsFile{""};

  std::string material{""};

  bool debugMode{true};

  bool isStrictMode{false};
};

/**
 *
 */
class WorkspaceManager {
 public:
  WorkspaceManager() {}
  WorkspaceManager(const WorkspaceManager&) = delete;
  void operator=(const WorkspaceManager&) = delete;

  static std::shared_ptr<Workspace> GetActiveWorkspace() {
    static auto activeWorkspace = std::make_shared<Workspace>();
    return activeWorkspace;
  }

//  static void SetActiveWorkspace(std::shared_ptr<Workspace> workspace) {
//    m_activeWorkspace = workspace;
//  }

 private:
//  static inline std::shared_ptr<Workspace> m_activeWorkspace {nullptr};
};

}  // namespace wsp
}  // namespace lsw