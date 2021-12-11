//
// Created by jeffrey on 11-12-21.
//
#pragma once

namespace lsw {
namespace glrs {

class IFramebuffer {
 public:
  virtual void bind() = 0;
  virtual void nextPass() = 0;
  virtual void apply() = 0;

  virtual void initialize() = 0;

  virtual void setSize(int width, int height) = 0;
  virtual void resize(int width, int height) = 0;
};

}  // namespace glrs
}  // namespace lsw