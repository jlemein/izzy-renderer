//#ifndef ARTIFAX_ECSFACTORY_H
//#define ARTIFAX_ECSFACTORY_H
//
//#include <entt/entt.hpp>
//#include <entt/fwd.hpp>
//
//namespace affx {
//
//namespace geo {
//struct Mesh;
//struct Curve;
//struct Scene;
//}
//
//namespace ecs {
//struct Shader;
//
//class EcsFactory {
//  entt::registry &m_registry;
//
//public:
//  EcsFactory(entt::registry &registry);
//
//  // primitive entity creations
//  /// @brief Creates a simple barebone entity containing minimum components
//  entt::entity makeEntity(std::string name = "");
//
//  entt::entity makeMesh(const geo::Mesh& mesh);
//
//  entt::entity makeCurve();
//
//  entt::entity makeRenderable(geo::Mesh &&mesh, const ecs::Shader &shader);
//  entt::entity makeRenderable(geo::Curve &&curve, const ecs::Shader &shader);
//
//  entt::entity makeScene(geo::Scene &);
//
//  entt::entity makeTexture();
//
//  entt::entity makeRectangularGrid(float size = 10.0F, float spacing = 1.0F);
//
//  entt::entity makeDebugVisualization(entt::entity target);
//};
//
//} // end of package
//} // end of enterprise
//#endif // ARTIFAX_ECSFACTORY_H
