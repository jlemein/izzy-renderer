#include <georm_fontsystem.h>
#include <filesystem>
using namespace lsw::georm;
namespace fs = std::filesystem;

FontSystem::FontSystem(const char* workspaceDir)
  : m_workspaceDir(workspaceDir) {}

std::vector<std::string> FontSystem::getAvailableFonts() {
  fs::path p = fs::path {m_workspaceDir} / "fonts";
  std::vector<std::string> fonts;

  for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it) {
    auto filePath = it->path().filename();
    if (filePath.is_relative()) {
      filePath = p / filePath;
    }
    fonts.emplace_back(filePath.c_str());
  }

  return fonts;
}