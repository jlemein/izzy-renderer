//
// Created by jlemein on 23-02-21.
//

#ifndef RENDERER_ECS_MATERIALSYSTEM_H
#define RENDERER_ECS_MATERIALSYSTEM_H

namespace affx {
namespace ecs {

/**!
 * A material system converts an assigned generic material to a render system
 * specific material description.
 * The generic material description is geo::Material and is read out as part of
 * blender scene description. It is the one that is altered in the UI, the one
 * that users interact with when changing textures and settings.
 *
 * The end result is that a geo::Material is filled
 */
class MaterialSystem {
public:
  void init();
  void update(float time, float dt);

};

} // end of package
} // end of enterprise
#endif // RENDERER_ECS_MATERIALSYSTEM_H
