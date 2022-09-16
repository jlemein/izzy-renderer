#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace izz {

struct Font {
  std::string name;
  int preferredSize;
};

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

  void addFont(std::filesystem::path fontFile, int preferredSize = 20);

  std::vector<Font> getAvailableFonts();

 private:
  std::vector<Font> m_availableFontFiles;
};

} // namespace izz