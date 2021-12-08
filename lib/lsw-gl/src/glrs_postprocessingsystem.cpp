//
// Created by jeffrey on 08-12-21.
//
#include <glrs_postprocessingsystem.h>
#include <glrs_rendersystem.h>
#include <ecsg_scenegraph.h>

using namespace lsw::glrs;

PostprocessingSystem::PostprocessingSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<glrs::IMaterialSystem> materialSystem)
  : m_sceneGraph{sceneGraph}
  , m_materialSystem{materialSystem} {}

void PostprocessingSystem::initialize() {

}
