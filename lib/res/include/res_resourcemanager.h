//
// Created by jlemein on 12-01-21.
//

#ifndef GLVIEWER_RES_RESOURCEMANAGER_H
#define GLVIEWER_RES_RESOURCEMANAGER_H

class Resource {};

class ResourceLoader {
public:
  void loadResource(const std::string& location);

  /**!
   * @return true if the resource loader is able to load the specified resource.
   */
  std::vector isLocationSupported(const std::string& location);
};

class ResourceManager {
public:
  /**!
   * Registers or adds a resource loader to the pool of resource loaders.
   * @param loader
   * @return a unique resource id corresponding to the loader.
   */
  ResourceId registerLoader(const ResourceLoader& loader) = 0;

  /**!
   * Removes the loader from the pool of resource loaders.
   * @param loaderId Resource id of the loader, retrieved when registering.
   */
  void removeLoader(ResourceId loaderId) = 0;


  Resource getResource();

  /**!
   * Similar to get resource, except the resource is loaded when used for the
   * first time. Using means whenever the resource is dereferenced.
   *
   * A lazy resource can be converted to ordinary @see Resource, but not back.
   * It is possible to store lazy resources in a lazy resource group, so that
   * all resources can be explicitly triggered to be loaded in memory.
   * @return
   */
  LazyResource getLazyResource();
};

#endif // GLVIEWER_RES_RESOURCEMANAGER_H
