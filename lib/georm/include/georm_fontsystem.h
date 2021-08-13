#pragma once

#include <vector>
#include <string>

namespace lsw {
namespace georm {

class FontSystem {
 public:
  FontSystem(const char* workspaceDir);

  std::vector<std::string> getAvailableFonts();

 private:
  std::string m_workspaceDir;
};

}
}