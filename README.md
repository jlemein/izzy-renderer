# Izzy Renderer

The Izzy renderer is a flexible lightweight renderer for Linux. The renderer is a rasterized renderer using OpenGL.
The renderer can render a diverse collection of scene file formats.
The renderer does not require much apriori knowledge. 
The renderer is designed to facilitate experimentation of rendering techniques. Therefore it is set up in a very
flexible way using an ECS scene graph. 

There are two ways to use Izzy renderer.
1. By making use of a scene file and materials file.
2. By using the renderer as a library. For examples, refer to the `examples` subfolder.

To use Izzy renderer with the first mode, run the following command:

`izzyrender --scene=hello.fbx --materials=materials.json`

Izzy renderer takes two input arguments:
* `--scene` Any of the supported scene file formats, such as *.fbx or *.collada files.
* `--materials` Materials file, which is a JSON file mapping the materials in the scene file to concrete shaders.

## Screenshots
![Rendering a teapot](doc/screenshot_teaset.png)


The following examples are delivered as part of the renderer:
* Normal mapping example
* Parallax mapping example

Aside from the scene file and materials file oriented approach, the library can be used as a C++ library as well.
Take a look at the API documentation and the hello world demo code on how to get started
using the library.

The library has been tested on:
* Ubuntu 18.04
* Ubuntu 20.04 (with AMD Ryzen 5700G)

# Architecture overview

> :warning: the current libraries will be refactored soon to reflect the design described below.

The codebase is in transformation right now. The library structure will be simplified. The following libraries are distinguished:
* `lsw` Core components needed for a rendering system. Think about materials, scene file representations, transformation matrices, light sources.
* `lsw::gl` GL render system. Contains openGL specific code for rendering. Aside from this library, nothing deals with OpenGL. In future, there will be Vulkan support.
* `lsw::gui` GUI system. Any
* `lsw::geo` Additional geometry objects.

# Practical examples

###  Loading a scene file

```shell
auto scene = resourceManager->getSceneLoader()->loadScene(workspace->sceneFile);
sceneGraph->makeScene(*scene, ecsg::SceneLoaderFlags::All());
```

If the loaded scene is too big, then scale the whole scene by scaling the root node.
```shell
ecs::TransformUtil::Scale(scene->rootNode()->transform, .20);
```

If you also want to generate smooth tangent, bitangent and normals
```shell
for (auto& mesh : scene->m_meshes) {
  geo::MeshUtil::GenerateTangents(*mesh);
  geo::MeshUtil::ConvertToSmoothNormals(*mesh);
}
```

### Adding a light source

Adding a directional light source, named "Sun" from direction (0, 1, 1).
```shell
sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
```

Adding a point light:
```shell
auto ptLight = sceneGraph->makePointLight("PointLight", glm::vec3(1.F, 1.0F, -1.0F));
ptLight.get<ecs::PointLight>().intensity = 4.0F;
```

#### Visualizing a light source

Usually you also want to render the light source, for example to visualize the point light source, to add bloom effects
or to make the light into an area light. This is trivial. Just add a `geo::Mesh` component to the same entity as the point
light source. The lighting system will add a `geo::Material` component to the entity if it does not have one. The default
lighting shader must be set before the light system is initialized:
```shell
lightSystem->setDefaultPointLightMaterial(m_materialSystem->makeMaterial("MyPointLight"));
```

If you want to use
a custom shader for the light source, make sure to assign a material before the light system is initialized or overwrite
the material later on in the simulation.





# API Documentation

### Build

Requirements:
* GLEW libraries - `sudo apt install libglew-dev`
* Conan 1.40+ - https://docs.conan.io/en/latest/installation.html
* CMake 3.15+
* C++17

Project makes use of Git LFS.


## Resolving materials and textures

Every renderer is may support different advanced features. It is for the time being impractical to have
a single unified material file format that supports all features of modern renderers. 
Most 3d models, however, provide some basic support for material settings, such as diffuse and specular colors, roughness values and
texture information.
Izzy Renderer derives as much as it can from the 3d models and render it to the best of it's knowledge.
If you want fancier results, then you need to use the materials file. The material file is used to override the default
material properties. Also the material file is used to express parameters to complex rendering techniques. The material
file is in that sense a material file format for the Izzy Renderer.

Resources are always loaded relative from the point of declaration.
* Textures specified in the scene file (as part of the fbx file) are resolved as part of the location of the scene file.
* Textures specified in the materials file are resolved from the location of the materials file.

Most meshes make use of external data, such as textures, animation files, lookup data, etcetera. By default, relative paths in the model
are resolved to the directory where the mesh is located.
If the relative path cannot be found, it is resolved to the workspace.

If the workspace does not contain the asset, it is resolved to the home directory.

If not found, it throws an error if --strict is an argument.

Otherwise, it finds suitable default values so that the renderer at least renders something.


The Izzy renderer is written with the scene graph implemented as an entity component systems (ECS).
The renderer consists of the following major systems:
* Render system
  * Texture system
  * Shading system: Part of the render system, dealing with compilation and managing shader properties.
* Resource manager
* Material system
  * Therefore the material is responsible to provide all the information that is required to render a specified effect
  * Textures
* Texture system: responsible for loading textures and keeping a registry of texture loaders.


## Material system

A material is described the visual attributes that are required to render a specific geometry.


# Project overview

The project is built using Cmake:
* cmake for building and generation of a source code project. Tested with make files.
* **ctest** for running the unit tests
* cmake install for installing the project
* **cpack** for packaging the install process as a GUI installer.

## Running unit tests

Make sure to generate the project with unit tests included:

```asm
    # if not already performed
    mkdir build && cd build

    cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
    ctest
```

## Installing from source

```asm
git clone repo
cd renderer && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt
make -j8
(sudo) make install # use sudo only when installing to /usr/bin
```

## Creating an installer

CPack is used to create a GUI installer. QTIFW is used as the cpack generator to create the installer.
Make sure that QtIFW is installed and that the environment variable is set (either using ~/.bashrc or using the
`export ROOT_QTIFW=<qtifw_root_directory>`).

For linux follow the instructions:

```asm
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cpack
```

# Object creation

* **PrimitiveFactory**: factory for creating simple objects, planes, triangles, boxes, spheres, cubes, cylinders, etcetera. A primitive factory produces a mesh object to which additional operations can be performed using the mesh utilities.

# Utilities

A entity component approach seperates data and logic. Applying operations to the data is done using appropriate utility functions.
Creating meshes can be done by loading in scene files, or by generating them using utilities. After meshes are created or loaded from file, they often lack essential data Therefore a couple of 
fingertips.

* **Mesh utilities** (`geo::MeshUtil`): essential for transforming raw mesh data. This includes generating normals and tangent data. Changes are applied to the mesh data itself, meaning that no computation logic is needed after a mesh utility is used. 
* **Transform utilities** (`ecs::TransformUtil`): essential for applying transformations to entities, such as scaling, rotating and moving objects.

# Special thanks

Special thanks to:
* James Ray Cock (Texturing), Jurita Burger (Graphic Design), Rico Cilliers (Model) - for the tea set model from Polyhaven.