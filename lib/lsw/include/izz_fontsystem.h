#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace lsw {

/**
 * Font system handles the loading and managing of fonts.
 * For now this is a simple place to add and retrieve font files.
 */
class FontSystem {
 public:
  /**
   * Constructor
   */
  FontSystem() = default;

  /**
   * Reads the directory and adds the fonts in the directory to the font system.
   * @param directory   [in] Directory containing font files (.TTF).
   * @return The number of fonts successfully read in.
   */
  int addFontsFromDirectory(std::filesystem::path directory);

  std::vector<std::string> getAvailableFonts();

 private:
  std::vector<std::string> m_availableFontFiles;
};

} // namespace lsw