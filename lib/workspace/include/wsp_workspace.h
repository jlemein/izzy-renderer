#pragma once

#include <string>

namespace lsw {
namespace wsp {

/**
 * @brief The workspace encapsulates the working directory of the application. All changes, or files that are saved
 * are being saved in the workspace directory (unless the user specifies otherwise via GUI actions).
 * The application will only save to .workspace.tmp, untill saved then .workspace.tmp becomes .workspace.
 *
 * The workspace is an abstraction to the working directory. This simplifies loading files, and understanding from where
 * the application runs.
 * The workspace directory is by default the working directory from where the application is started.
 */
struct Workspace {
  /// @brief Location of installation directory, which should be ${WERA3D_HOME}.
  std::string installDir{""};  // home directory of wera3d containing default fallback materials. Not meannt to be adjusted

  /// @brief Location of workspace directory if specified on command line. Otherwise it's the current working directory.
  std::string path{""};     // by default the location from where the application is run.

  /// @brief Scene file to be loaded, as specified on the command line
  std::string sceneFile{""};

  /// @brief
  std::string materialsFile{""};
  
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

  static Workspace& GetWorkspace() {
    static Workspace instance;
    return instance;
  }
};



static std::string R(const char* path) {
  auto& workspace = WorkspaceManager::GetWorkspace();
  return workspace.path + "/" + path;
}

}  // namespace wsp
}  // namespace lsw