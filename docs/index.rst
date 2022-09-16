========
Izzy Renderer
========

Izzy renderer is my personal project to understand the details of rendering
technologies. As such, it is designed in a way so that the engine can
easily be configured for different rendering strategies. It should be easy
to understand and extend. The engine is currently still written using OpenGL.

The following rendering systems are available

- Forward renderer: used exclusively to render the scene in a single pass.
- Deferred renderer: used for sophisticated effects.

Both render systems have their advantages. One notable difference is that
transparency is way easier to accomplish using forward rendering, however
rendering a scene with many lights is better suited for deferred rendering.

""""""""""
Where to start
""""""""""
Izzy renderer is composed of modules. This is done to decouple the system and therefore make
it easy to update or change modules in the future. For example, this should make it easier to
transition from OpenGL to Vulkan, or from one animation system to the other.

Every module starts with the ``izz::`` namespace. The following modules are available:

+---------------------------+---------------------------------------------------------------------------------------+
| Module (namespace)        | Description                                                                           |
+===========================+=======================================================================================+
| ``izz::``                 | The core izzy system, containing most of the basic data structures and functionality. |
| ``izz::gl``               | The OpenGL rendering system.                                                          |
| ``izz::gui``              | The Izzy GUI system (based on ImGui).                                                 |
| ``izz::anim``             | Animation system                                                                      |
+---------------------------+---------------------------------------------------------------------------------------+
For example, the core part of the library is izz::, OpenGL related code is part of the izzgl,
and the GUI component is part of izzgui.