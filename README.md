# Wera3D Renderer

The Wera3D renderer is a flexible lightweight renderer for Linux. The renderer is a rasterized renderer using OpenGL.
The renderer can render a diverse collection of scene file formats.
The renderer does not require much apriori knowledge. 

To use the wera3d renderer, run the following command:

`wera3d --scene=hello.fbx --materials=material.json`

Wera3d takes two input arguments:
* `--scene` Any of the supported scene file formats, such as *.fbx or *.collada files.
* `--materials` Materials file, which is a JSON file mapping the materials in the scene file to concrete shaders.


The following examples are delivered as part of the renderer:
* Normalmapping example
* Parallax mapping example

Aside from the scene file and materials file oriented approach, the library can be used as a C++ library as well.
Take a look at the API documentation and the hello world demo code on how to get started
using the library.

The library has been tested on:
* Ubuntu 18.04
* Ubuntu 20.04 (with AMD Ryzen 5700G)


# API Documentation

### Build

Requirements:
* GLEW libraries - `sudo apt install libglew-dev`
* glslc

Project makes use of Git LFS.

### Preconditions

1. Extract the wera3d at your location of choice. Suggestions are in: `~/opt/wera3d-1.0`.
2. Make sure to add the bin directory to the $PATH environment variable, by editing the `~/.bashrc` file.
3. Run source ~/.bashrc to make active the changes you just made to the bash configuration.
4. Set the environment variable `WERA3D_HOME` to the root wera3d location, e.g. `~/opt/wera3d-1.0`. The environment variable is used to resolve default materials and shaders and to write out configuration files.
5. Set the environment variable `WERA3D_WORKSPACE` to resolve your materials and assets from the workspace directory. If you do not specify this variable, and not specify it on the command line the home directory will be used to resolve materials.
   1. Multiple workspaces can be set using a comma separated list, e.g: ```set WERA3D_WORKSPACE='/home/user/mydir1:/home/user/mydir2```. The workspace order determines the priority where materials are resolved. The first mentioned takes precedence.
   2. If you additionall provide a workspace argument, the workspace environment variables are ignored.
   3. If you don't want the renderer to use fallback mechanisms, use the command line argument: `--strict`.

To summarize the instructions:


```wera3d -s scene.fbx```
Renders an arbitrary scene file placed at the origin. The materials for the meshes are rendered using a default
uber shader. Disney shader will be used in future release. 

```wera3d -s scene.fbx -m materials.json```

## Basic usage

To render a scene the renderer needs a scene file and material assignments.
A scene file will usually already consist of material assignments and properties.
However, those assignments are tightly coupled to the underlying render system. Those 
properties might not fit with the actual rendering implementation of Wera3d. Therefore
the material properties are described in specific materials files.

File conventions:

* Let's assume the scene file is called car.fbx.
* The car does assign material names to individual meshes. Multiple meshes can exist referring to the same material (name).
* The material name is used to lookup the material in the Wera3d renderer.
* Wera3d consist of a material collection.
  * By default the default_materials.json is used in the `WERA3D_HOME` path environment variable.
  * By supplying a materials argument to the renderer, the materials can be overridden.

Example usages:

| Command                                           | Description  |
|---------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------|
```wera3d model.fbx```                              | Renders a model using materials from the default material collection.
```wera3d -s model.fbx -m materials.fbx```          | Renders a model using materials from the provided materials file. If not found, it still uses the default material collection.
```wera3d -s model.fbx -m materials.fbx --strict``` | Renders a model, using only the materials.fbx. No default material collection is used.


## Resolving textures

Most meshes make use of external data, such as textures, animation files, lookup data, etcetera. By default, relative paths in the model
are resolved to the directory where the mesh is located.
If the relative path cannot be found, it is resolved to the workspace.

If the workspace does not contain the asset, it is resolved to the home directory.

If not found, it throws an error if --strict is an argument.

Otherwise, it finds suitable default values so that the renderer at least renders something.


# ds

In this case the renderer uses the built in


If no material file is provided, the scene will be rendered using a standard universal shader.
This universal shader is found in the install directory under `${WERA3D_HOME}/shaders`.

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

A resource manager is a thin she
## Material system

A material is responsible to provide all the information that is required to render a specified effect.

The `geo::Material` component is an importa
A material system handles the information and is responsible for processing material components. A material editor's responsibilities include:
* Loading the textures.
* Knowing when to prepare and pre-load entities, based on current state of the scene graph.
* Unloading of materials.

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

