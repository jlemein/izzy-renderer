#pragma once

/**
 * Deals with handling program arguments. A program can be started like:
 *
 * A workspace consists of the assets in the following structure:
 * workspace/
 *      assets/
 *          models/
 *          textures/
 *          fonts/
 *          shaders/
 *          materials/
 *
 * lswrender --workspace=~/.lswrender --materials m.json --scene=mycar.fbx
 */
class ProgramOptions {
 public:
  ProgramOptions(const char* defaultWorkspaceDir = "~/.lswrender/workspace")
    : m_workspaceDir(defaultWorkspaceDir)
    , m_sceneFile("") {}

  /**
   *
   * @param argc
   * @param argv
   * @return True if parse successful, false otherwise.
   */
  bool parseArguments(int argc, char* argv[]) {
    if (argc < 2) {
      spdlog::error("Not enough arguments provided");
      return false;
    }

    m_workspaceDir = argv[1];

    if (!m_workspaceDir.empty() && m_workspaceDir[0] == '~') {
      const char* home = getenv("HOME");
      if (home == nullptr) {
        throw std::runtime_error(
            "Workspace dir uses ~, but $HOME environment variable not set. Cannot expand the users' home directory.");
      }
      m_workspaceDir = std::string{home} + m_workspaceDir.substr(1);
      spdlog::info("Workspace dir: {}", m_workspaceDir);
    }
    //    m_sceneFile = argv[2];
    return true;
  }

  /// @brief A workspace dir marks the place where the assets are stored
  const char* getWorkspaceDir() {
    return m_workspaceDir.c_str();
  }

  const char* getSceneFilePath() {
    return m_sceneFile.c_str();
  }

 private:
  std::string m_workspaceDir;
  std::string m_sceneFile;
};