#include <izz_fontsystem.h>

#include <filesystem>
#include <fmt/format.h>

using namespace lsw;
namespace fs = std::filesystem;

void FontSystem::addFont(std::filesystem::path path, int preferredSize) {
  if (fs::exists(path)) {
    m_availableFontFiles.emplace_back(Font{path, preferredSize});
  } else {
    throw std::runtime_error(fmt::format("Could not load font: {}", path.c_str()));
  }
}

std::vector<Font> FontSystem::getAvailableFonts() {
  return m_availableFontFiles;
}