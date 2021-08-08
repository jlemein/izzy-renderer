# Artifax Renderer

The artifax renderer is written using entity component systems (ECS).
The Lemon Shite Renderer consists of the following major systems:
* Render system
  * Textures 
* Resource manager
* Material system
  * Therefore the material is responsible to provide all the information that is required to render a specified effect
  * Textures

## Resource manager

A resource manager managed the administration of all resources in the scene, and outside of the scene.
All resources involve assets such as scene files, audio files, video files, images, textures, materials, but also ordinary text files.
A resource manager is a superficial instance by itself. It does not know much about each of the assets.
The responsibility of a resource manager is as follows:
* Forwards calls to the corresponding resource factory. The resource manager abstracts away the underlying resource systems for clients using the resource manager.
* Maintains the collection of created resources, but it does not decide on the lifetime of an object. This is the responsibility of the resource system.
* Issues unique ids. 

A **resource system** is involved with managing the resource components.

handles all external resources loaded in the scene.
It is composed of a set of resource factories. Each resource factory is able to create a specific resource. The resource manager makes sure the right factory is created.
A resource factory is only responsible for resource creation. 

* Provides an interface to load external resources, such as image files, HDR files, sound files, video files, scene files. The creation is forwarded to the proper factory method.
* Assigning resource id's.
* Managing or caching resources for faster re-use.
* Resource sharing of the same external resource (i.e. textures) or sound files.

A material is not an external resource, although it's consti
## Material system

A material is responsible to provide all the information that is required to render a specified effect.

The `geo::Material` component is an importa
A material system handles the information and is responsible for processing material components. A material editor's responsibilities include:
* Loading the textures.
* Knowing when to prepare and pre-load entities, based on current state of the scene graph.
* Unloading of materials.

