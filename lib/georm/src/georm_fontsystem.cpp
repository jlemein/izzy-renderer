#include <georm_fontsystem.h>
#include <filesystem>
using namespace lsw::georm;
namespace fs = std::filesystem;

FontSystem::FontSystem(const char* workspaceDir)
  : m_workspaceDir(workspaceDir) {}

std::vector<std::string> FontSystem::getAvailableFonts() {
  std::vector<std::string> fonts {};

  fs::path fontsDirectory = fs::path{m_workspaceDir} / "fonts";
  if (!fs::exists(fontsDirectory)) {
    return fonts;
  }

  for (fs::directory_iterator it(fontsDirectory); it != fs::directory_iterator(); ++it) {
    auto filePath = it->path().filename();
    if (filePath.is_relative()) {
      filePath = fontsDirectory / filePath;
    }
    fonts.emplace_back(filePath.c_str());
  }

  return fonts;
}