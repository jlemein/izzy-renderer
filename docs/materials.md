# Materials

## Selection of shaders

Vertex shaders

* `standard_perspective.vs.glsl` basic vertex shader passing the vertex attributes in world space using MVP matrix. 

| Shader name        | 0               | 1             | 2         | 3              | Notes                                                                                 |
|--------------------|-----------------|---------------|-----------|----------------|---------------------------------------------------------------------------------------|
| `standard.vs.glsl` | `vec3` position | `vec3` normal | `vec2` uv | `vec3` tangent | Standard vertex processing shader passing the position, normal, uv and tangent data.|
|                    |                 |               |           |                |                                                                                       |
|                    |                 |               |           |                |                                                                                       |
|                    |                 |               |           |                |                                                                                       |
|                    |                 |               |           |                |                                                                                       |


## Interface methods

A uniform buffer has the following interface methods:
* `allocate` allocates a new structure of uniform data. This buffer structure should match the uniform buffer object in the shader.
* `destroy` destroys the uniform data structure that was created by an earlier call to `allocate`.
* `onInit` allows for initialization of the buffer data.
* `onFrameStart` is called every start of the frame, **only once** per used uniform buffer parameter. 
Even if multiple materials exist using the same type of uniform buffer. This is handy to calculate data that 
is shared among multiple materials, such as the view-projection matrix or to compute the lights.
* `onUpdate` is called for every material to update generic data.
* `pushData` is used to store the data in the uniform buffer data structure, for each material. 
Any precomputations that are handled by the onFrameStart or 

# FAQ

#### No uniform buffer manager defined for uniform buffer 'ForwardLighting'.

If you create your own custom uniform buffer structure, you need to write a manager class that deals
with creating and setting the values. 
* See `izz::ufm::ModelViewProjectionManager` as an example for built-in supported structures.
* You can also pass your own fixed values.

#### Cannot access ModelViewProjection matrix for material 'table_cloth_0'. Does shader have 'ModelViewProjection' uniform buffer?
In order for the engine to find your uniform buffers in the shader, you must assure:
* Make sure `materials.json` contains the uniform buffer as part of it's properties definition.
* That the uniform definition is present in the shader file.
* That at least one of the variables of the uniform buffer is used.