#include <entt/entt.hpp>
#include <entt/fwd.hpp>

#ifndef ARTIFAX_ECSFACTORY_H
#define ARTIFAX_ECSFACTORY_H

namespace artifax {

namespace shp {
struct Mesh;
struct Curve;
}

namespace ecs {
struct Shader;
}

class EcsFactory {
  entt::registry &m_registry;

public:
  EcsFactory(entt::registry &registry);

  entt::entity makeMesh(std::string sceneFile);

  entt::entity makeCurve();

  entt::entity makeRenderable(shp::Mesh&& mesh, const ecs::Shader& shader);
  entt::entity makeRenderable(shp::Curve&& curve, const ecs::Shader& shader);


  entt::entity makeTexture();

  entt::entity makeRectangularGrid(float size = 10.0F, float spacing = 1.0F);

  entt::entity makeDebugVisualization(entt::entity target);


};
} // namespace artifax
#endif // ARTIFAX_ECSFACTORY_H
