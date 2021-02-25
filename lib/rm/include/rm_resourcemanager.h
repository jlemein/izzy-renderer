//
// Created by jlemein on 25-02-21.
//

#ifndef RENDERER_RM_RESOURCEMANAGER_H
#define RENDERER_RM_RESOURCEMANAGER_H

/**!
 *  A resource manager manages the lifetime of resource objects, both
 *  construction and destruction.owns the created and loaded resources.
 *
 */
class ResourceManager {
public:
  geo::Texture* getTexture();
  geo::Scene* getScene();
  
};

#endif // RENDERER_RM_RESOURCEMANAGER_H
