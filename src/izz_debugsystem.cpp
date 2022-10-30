//
// Created by jlemein on 01-12-20.
//
#include <geo_primitivefactory.h>
#include <izz_debug.h>
#include <izz_debugshapefactory.h>
#include <izz_debugsystem.h>
#include <izz_izzy.h>
#include <izz_relationshiputil.h>

using namespace izz;

DebugSystem::DebugSystem(izz::Izzy& izzy, entt::registry& registry)
  : m_izzy{izzy}
  , m_registry{registry} {}

void DebugSystem::init() {
  auto view = m_registry.view<Debug>();
  for (auto e : view) {
    Debug debug = view.get<Debug>(e);

    auto pName = m_registry.try_get<Name>(e);
    std::string name = pName ? pName->name : "Unnamed";

    DebugModel model = DebugShapeFactory(*m_izzy.materialSystem, *m_izzy.meshSystem).MakeModel(debug.shape, m_registry, e);

    for (int i = 0; i < model.renderable.size(); ++i) {
      std::string debugName = std::string{"Debug#"} + std::to_string(i) + " " + name;

      auto sge =
          m_izzy.entityFactory->makeRenderable(debugName, model.renderable[i], model.transformations[i].localTransform, izz::gl::RenderStrategy::FORWARD);
      //      sge.get<izz::gl::ForwardRenderable>().isWireframe = model.isWireframe;
      RelationshipUtil::MakeChild(m_registry, e, sge.handle());
    }
  }
}
