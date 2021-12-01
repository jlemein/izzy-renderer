#include <izz_fontsystem.h>

#include <filesystem>
using namespace lsw;
namespace fs = std::filesystem;

int FontSystem::addFontsFromDirectory(std::filesystem::path directory) {

  // store number of fonts before, to calculate added number of fonts at the end of method
  int prevNumberFonts = m_availableFontFiles.size();

  if (fs::exists(directory)) {
    for (fs::directory_iterator it(directory); it != fs::directory_iterator(); ++it) {
      auto filePath = it->path().filename();
      m_availableFontFiles.emplace_back(filePath.is_relative() ? directory / filePath : filePath);
    }
  }

  int numberFonts = m_availableFontFiles.size();
  return numberFonts - prevNumberFonts;
}

std::vector<std::string> FontSystem::getAvailableFonts() {
  return m_availableFontFiles;
}