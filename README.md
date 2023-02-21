# Izzy Renderer

The Izzy renderer is a flexible lightweight renderer for Linux. The renderer is a rasterized renderer using OpenGL.
The renderer can render a diverse collection of scene file formats.
The renderer does not require much apriori knowledge.
The renderer is designed to facilitate experimentation of rendering techniques. Therefore, it is set up in a very
flexible way using the ECS architectural design pattern.

Izzy came to the world because of my fascination for computer graphics. I wanted a controlled environment to write
shaders and test rendering techniques. Most popular engines do not provide the fine grained control you want. And so
Izzy
renderer is born. For now Izzy makes use of OpenGL, but Vulkan is on the radar. Also, the idea is to support physics
based renderer as well.

There are two ways to use Izzy renderer.

1. By making use of a scene file and materials file.
2. By using the renderer as a library. For examples, refer to the `examples` subfolder.

To use Izzy renderer with the first mode, run the following command:

`izzyrender --scene=hello.fbx --materials=materials.json`

Izzy renderer takes two input arguments:

* `--scene` Any of the supported scene file formats, such as *.fbx or *.collada files.
* `--materials` Materials file, which is a JSON file mapping the materials in the scene file to concrete shaders.

The library has been tested on:

* Ubuntu 18.04
* Ubuntu 20.04 (with AMD Ryzen 5700G)

## Screenshots

Tea set rendered using Blinn-Phong shading and a diffuse plane, acting as table cloth.
All materials use textures for the albedo and normal maps.
![Rendering a teapot](doc/teaset_on_tablecloth.png)

Brass vase is rendered using Blinn-Phong shading once again. The shininess can be observed clearly.
![Brase Vass](doc/brase_vass.png)

Brass vase is rendered using Blinn-Phong shading. The effect of normal mapping is clearly visible in this
rendering, since the model itself is a smooth barrel.
![Barrel](doc/barrel.png)

# Getting started

Take a look at `examples/m_izzyrender.cpp`. Most of the code will look intuitive enough to start exploring izzy
renderer.

For more detailed explanations:
1. Materials
2. [The material system](docs/materials.md)
   3. The materials file.
   4. Writing custom shaders.
3. The texture system
   4. Loading textures from file.
   5. Allocating textures and depth buffers.
3. 

### Introduction

The renderer is designed on the entity component system (ECS) pattern. An ECS system builds on entities. Items or
subjects in the scene are called **entities**.
Entities are just a number. There is nothing more to it. By adding **components** to the entities you can control the
behavior of an entity.
Components by definition only contain data. The behavior of the entities is implemented in **systems**. Hence the name
"entity component system".

This is a fundamentally different technique compared to object oriented (OO) design. OO tries to encapsulate
implementation
details behind interfaces and classes., whereas ECS works with public data. ECS is mainly used for simulations or game
applications, where a lot of different entities are present with varying behaviors. More on ECS systems can be found
on [skypjack's github page](https://github.com/skypjack/entt), writer and maintainer of the EnTT library that is used in
izzy renderer as well.

### Setting up systems

The first thing to onBeginFrame are the systems. The systems are the heart of the simulation. Without systems nothing will
happen.
Izzy renderer will contain a huge amount of systems. Setting up the systems can be tricky because the order is
important.
In future this will likely be hidden in factory methods.

Setting up the systems for an interactive renderer:

```shell
namespace {
std::shared_ptr<Workspace> programArguments{nullptr};
std::shared_ptr<ResourceManager> resourceManager{nullptr};
std::shared_ptr<MaterialSystem> materialSystem{nullptr};
std::shared_ptr<ecsg::SceneGraph> entityFactory{nullptr};
std::shared_ptr<glrs::RenderSystem> renderSystem{nullptr};
std::shared_ptr<SceneLoader> sceneLoader{nullptr};
std::shared_ptr<FontSystem> fontSystem {nullptr};
std::shared_ptr<gui::GuiSystem> guiSystem {nullptr};
}  // namespace

void setupSystems() {
  resourceManager = make_shared<ResourceManager>();
  entityFactory = make_shared<ecsg::SceneGraph>();

  auto textureSystem = make_shared<TextureSystem>();
  textureSystem->setTextureLoader(".exr", std::make_unique<ExrLoader>(true));
  textureSystem->setTextureLoader(ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<StbTextureLoader>(true));
  resourceManager->setTextureSystem(textureSystem);

  materialSystem = make_shared<MaterialSystem>(entityFactory, resourceManager);
  resourceManager->setMaterialSystem(materialSystem);

  sceneLoader = make_shared<SceneLoader>(textureSystem, materialSystem);
  resourceManager->setSceneLoader(sceneLoader);

  renderSystem = make_shared<glrs::RenderSystem>(entityFactory, materialSystem);
  fontSystem = make_shared<FontSystem>();
  guiSystem = make_shared<gui::GuiSystem>();
}
```

### Loading a scene file

```shell
auto scene = sceneLoader->loadScene("models/mymodel.fbx");
entityFactory->makeScene(*scene);
```

The `makeScene` method has a second optional argument `ecsg::SceneLoaderFlags` that describes which components of the
scene file to load. By default, it loads geometry, materials and animations. If you also want to load the cameras and
light
sources from the scene file, you need to explicitly enable that, for example by specifying to load all data,
i.e. `ecsg::SceneLoaderFlags::All()`.

If the loaded scene is too large, then scale the entire scene by scaling the root node.

```shell
ecs::TransformUtil::Scale(scene->rootNode()->transform, .20);
```

If you also want to generate smooth normals, tangents and bitangents:

```shell
for (auto& vertexBufferId : scene->m_meshes) {
  geo::MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(*vertexBufferId);
  geo::MeshUtil::GenerateSmoothNormals(*vertexBufferId);
}
```

### Adding a light source

Adding a directional light source, named "Sun" from direction (0, 1, 1).

```cpp
entityFactory->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
```

Adding a point light:

```cpp
auto ptLight = entityFactory->makePointLight("PointLight", glm::vec3(1.F, 1.0F, -1.0F));
ptLight.get<ecs::PointLight>().intensity = 4.0F;
```

### Adding geometry

Izzy renderer comes with a `PrimitiveFactory` that can generate simple primitive meshes, such as spheres, boxes and
planes.
The primitive factory exists to facilitate easy spawning objects to the scene without needing to specify your own vertex
buffers, uv coordinates and normal data.

Generating a plane:

```cpp
auto plane = PrimitiveFactory::MakePlane("Plane", 15.0, 15.0); // plane with dimensions 15 x 15
auto blinnPhong = materialSystem->createMaterial("BlinnPhong");
entityFactory->addGeometry(plane, blinnPhong);
```

#### Visualizing a light source

Usually you also want to render the light source, for example to visualize the point light source, to add bloom effects
or to make the light into an area light. This is trivial. Just add a `geo::Mesh` component to the same entity as the
point
light source. The lighting system will add a `geo::Material` component to the entity if it does not have one. The
default
lighting shader must be set before the light system is initialized:

```shell
lightSystem->setDefaultPointLightMaterial(m_materialSystem->makeMaterial("MyPointLight"));
```

If you want to use
a custom shader for the light source, make sure to assign a material before the light system is initialized or overwrite
the material later on in the simulation.

# Material and effect system

The izzy renderer has materials and effects.

* **Material**: single pass rendering technique. It describes surface properties and is corresponding to a compiled
  shader program, consisting of vertex and fragment shader.
* **Effect**: multi pass rendering technique. An effect is composed of multiple materials. The connection between
  materials is described using a node graph.

Both materials and effects are uniquely described by a name in the `materials.json`.
The material name inside a 3d model file will correspond to either an effect or a material.

The following order is used to resolve the material or effect.

1. First, the effect system is queried. If the name matches an effect, the effect is returned.
2. If no effect is found, the material instance mapping is searched. If there is a material instance with the same name,
   the name gets mapped to the "concrete" material name. If not found, the "concrete" material name is set to the same
   name.
3. Given a (mapped) material name, the material system is queried. If found, a single pass effect is returned, with the
   single pass being the material searched for.

## Render order

The scene is rendered using a specific rendering strategy. Example rendering strategies are forward-rendering, or
deferred
rendering. The rendering strategy is in control on what and how to render. Note that it is not exclusively forward or
deferred rendering, but it can also be a hybrid strategy. The strategy is free to implement.

The rendering strategy imposes rules and restrictions on the renderable objects in the scene.
Usually the following order is used:

1. The rendering strategy decides upon a rendering effect to use.
2. Loop over all renderable objects

#### Adding post processing effects

Post-processing is supported via camera entities. A camera entity can have zero, one or multiple post-processing
effects assigned to it. This is done by assigning a `PostprocessCollection` component.
A postprocess collection component maintains an ordered list of post-processing effects. Each post-processing effect is
an entity
in itself and should be assigned the `Postprocess`, `Renderable` and `Material` component.

Example of adding two post-processing effects. Note that the post-processing effects are materials. They behave
the same as standard materials, except for the fact they have different uniform shader parameters.

```cpp
// onBeginFrame camera
auto camera = entityFactory->makeCamera("DummyCamera", 4);
auto grayscale = entityFactory->makePosteffect("GrayScale", *materialSystem->createMaterial("GrayScalePostEffect"));
auto vignette = entityFactory->makePosteffect("Vignette", *materialSystem->createMaterial("VignettePostEffect"));

// add post-processing effects. 
// i.e. multipass: first pass grayscale; second pass vignette.
camera.add<PosteffectCollection>({.posteffects = {grayscale, vignette}});
```

### Build

The software has been built using the following dependencies.

Requirements:

* GLEW libraries - `sudo apt install libglew-dev`
* Conan 1.42.0 - https://docs.conan.io/en/latest/installation.html
* CMake 3.21.1
* GCC 10

Project makes use of Git LFS.

## Resolving materials and textures

Every renderer is may support different advanced features. It is for the time being impractical to have
a single unified material file format that supports all features of modern renderers.
Most 3d models, however, provide some basic support for material settings, such as diffuse and specular colors,
roughness values and
texture information.
Izzy Renderer derives as much as it can from the 3d models and render it to the best of it's knowledge.
If you want fancier results, then you need to use the materials file. The material file is used to override the default
material properties. Also the material file is used to express parameters to complex rendering techniques. The material
file is in that sense a material file format for the Izzy Renderer.

Resources are always loaded relative from the point of declaration.

* Textures specified in the scene file (as part of the fbx file) are resolved as part of the location of the scene file.
* Textures specified in the materials file are resolved from the location of the materials file.

Most meshes make use of external data, such as textures, animation files, lookup data, etcetera. By default, relative
paths in the model
are resolved to the directory where the vertexBufferId is located.
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

A material describes the visual attributes to render a geometry. In Izzy renderer the material
system is built upon multiple layers.

When creating a material using the same shader file names, the engine will try to reuse an existing material program, because rebinding program is generally considered more costly than changing shader uniforms.

| C++ class                     | Description                                                                                                                                                                                               | Lives on |
|-------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------|
| `MaterialTemplate`         | Describes the complete material on CPU side. Used to instantiate the real GPU specific materials.                                                                                                         | CPU      |
| `MaterialInstanceDescription` | Inherits from `MaterialTemplate` and differs only in specified attributes. This makes it possible to define multiple materials in a vertexBufferId object, but still using the same Material program in the end. | CPU      |
| `Material`                    | Material program (accompanied with program id and allocated texture and uniform buffers.                                                                                                                  | GPU      |

## Texture system

The texture system owns the textures in the application. This makes it possible to facilitate
reuse of textures used by different components in the application. Also it makes it possible to track
memory usage. It facades the interface to the lower level rendering system (OpenGL).

A texture can be created via the texture system using any of the following calls:
* `createImageTexture(path)` loads a texture from disk located by the specified path.
* `createTexture(width, height)` allocates a texture without data. Used for GPU side rendering (shadow maps).
* `createDepthTexture(width, height)`

You will retrieve a pointer to the texture. If you created a texture you are expected to perform 
a call to the delete variant to signal memory can be freed.

Textures can be given names by calling `setName(name)`. This makes them more visible if an error
occurs or if the debugging tools are used.

To retrieve already allocated textures from the texture system, use one of the following calls.
* `getTextureById(id)`
* `getTextureByName(name)`

### Accessing texture data.

The texture data resides on the GPU. It is possible to fetch this data with a call to `fetchData()`. After this call
the texture data is accessible from the CPU. This is a costly operation, so use it wisely.

If texture data needs to be manipulated, you can fetch the texture data from the GPU. Then perform
your modifications and push the data back to the GPU. 
If you want to replace the existing data without modification, you can also set the data and push the data.

```
Texture* pTexture = m_textureSystem->createImageTexture("mytexture.png");
pTexture.fetchData();
pTexture.data = newData;
pTexture.pushData(); 
```

### Textures in shaders.

*To be added*




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

* **PrimitiveFactory**: factory for creating simple objects, planes, triangles, boxes, spheres, cubes, cylinders,
  etcetera. A primitive factory produces a vertexBufferId object to which additional operations can be performed using the vertexBufferId
  utilities.

# Utilities

A entity component approach seperates data and logic. Applying operations to the data is done using appropriate utility
functions.
Creating meshes can be done by loading in scene files, or by generating them using utilities. After meshes are created
or loaded from file, they often lack essential data Therefore a couple of
fingertips.

* **Mesh utilities** (`geo::MeshUtil`): essential for transforming raw vertexBufferId data. This includes generating normals and
  tangent data. Changes are applied to the vertexBufferId data itself, meaning that no computation logic is needed after a vertexBufferId
  utility is used.
* **Transform utilities** (`ecs::TransformUtil`): essential for applying transformations to entities, such as scaling,
  rotating and moving objects.

# Special thanks

Special thanks to:

* James Ray Cock (Texturing), Jurita Burger (Graphic Design), Rico Cilliers (Model) - for the tea set model from
  Polyhaven.