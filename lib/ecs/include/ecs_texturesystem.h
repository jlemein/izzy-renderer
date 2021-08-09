////
//// Created by jlemein on 06-11-20.
////
//
//#ifndef GLVIEWER_VIEWER_TEXTURESYSTEM_H
//#define GLVIEWER_VIEWER_TEXTURESYSTEM_H
//
//#include <ecsg_scenegraphentity.h>
//#include <entt/entt.hpp>
//#include <memory>
//
//namespace lsw {
//
//namespace viewer {
//class Viewer;
//} // namespace viewer
//
//namespace ecsg {
//class SceneGraph;
//} // namespace ecsg
//
//namespace ecs {
//
//class TextureSystem { //: public lsw::ecs::IViewerExtension,
//                      //public lsw::ecs::IRenderSubsystem {
//public:
//  TextureSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph);
//  ~TextureSystem() = default;
//
//  // @see IViewerExtension
//  void initialize();
//    // converts geo::Material components into respective entities having an
//    // ecs::Texture component.
//
//  void update(float time, float dt);
//
//  // @see IRenderSubsystem
//  void onRender(entt::entity entity);
//
//private:
//  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
//};
//
//} // end of package
//} // end of enterprise
//
//#endif // GLVIEWER_VIEWER_TEXTURESYSTEM_H
