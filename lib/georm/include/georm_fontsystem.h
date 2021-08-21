#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace lsw {
namespace georm {

class FontSystem {
 public:
  FontSystem(const char* workspaceDir);

  std::vector<std::string> getAvailableFonts();

 private:
  std::filesystem::path m_workspaceDir;
};

}
}