# Changelog

All notable changes to the Calamity Engine from Alpha 1.0 to Alpha 1.1.

## [Alpha 1.1]

### New Features & Enhancements

- **Multi-Window Support**: 
  - Restructured the engine's rendering to manage a collection of multiple concurrent window instances (`std::vector<std::shared_ptr<Window>>`), decoupling graphics rendering from a single global canvas.
  - Each window can be configured with its own root node, active camera, dimensions, presentation mode, clear color, and fullscreen state.
- **Dynamic Window Management**: 
  - Added options to the `Window` class to dynamically cycle fullscreen states, resize window dimensions, query supported display/window modes, and adjust logical rendering presentations at runtime.
- **Tracy Profiler Integration**:
  - Integrated Tracy profiler annotations (`ZoneScoped` and `FrameMark`) across main loops (Engine, Input, and Nodes) to enable visual performance profiling when compiling with Tracy support enabled.
- **Physics Raycasting**:
  - Added the `Raycast` class and `RaycastResult` struct to the physics engine to calculate raycast intersections in the Box2D physics simulation.
- **Collision Detection & Sensors**:
  - Added sensor support to shapes via `Shape::setSensor(bool)`, enabling overlap detection without triggering physical contact response.
  - Added collision signals (`collisionEnter`, `collisionExit`, `collisionHit`) directly on the base `PhysicsBody` class using the `Signal` system.
- **Mouse Hover Signals for Physics Bodies**:
  - Added `mouseEntered` and `mouseExited` signals on `PhysicsBody` to detect and handle mouse pointer hover states over collision shapes.
- **New Physics Shapes**:
  - Added `SegmentShape` and `Segment` definitions, expanding the physics shape options.
- **Base Node Visibility & State Control**:
  - Centralized the `visible` property to the base `Node` class (automatically applying to all child components and children nodes recursively) instead of having separate `visible` fields on individual rendering components (e.g., `Sprite`, `AnimatedSprite`, `Tilemap`, `ShapeSprite`, `Label`).
  - Added `enabled` (and `isEnabled()`) to the base `Node` class to control whether a node's update, input, and render loops are executed.
- **Engine-Level FPS Limiter**:
  - Added `maxFps` to `Engine` to limit framerate using `SDL_DelayNS` in the main update loop.
- **Input Event Modifier Flags**:
  - Added parsing of SDL modifier keys (`shiftPressed`, `altPressed`, `ctrlPressed`, `metaPressed`) in key events via `fromSDLmod`.
- **Vector2 Utilities**:
  - Added `lerp` and `distanceTo` methods to the `Vector2` struct.

### Fixes & Refactoring

- **Wall Sliding**:
  - Set player physics body material friction to `0.0f` in the platformer example, resolving the issue where characters stick to walls instead of sliding down.
- **Capsule Rendering**:
  - Fixed Capsule shape rendering in physics/shape rendering.
- **Restructured Physics Inheritance**:
  - Refactored physics bodies (`RigidBody`, `StaticBody`, `KinematicBody`) to inherit from a common `PhysicsBody` class, which handles shape creation, body cleanup, and signals.
- **Refactored Examples**:
  - Updated examples to use the new `PhysicsBody` structure.
  - Replaced explicit types with modern `auto` variables for cleaner code readability.
  - Refactored `examples/physics` to clean up architecture.
- **Console & Windows Build Conveniences**:
  - Relocated example asset copying in CMake using `add_custom_command` to copy assets directory to output directory, allowing invocation from any working directory/console.
  - Configured MSVC builds to hide the console window when building in Release configuration (`WIN32_EXECUTABLE $<CONFIG:Release>`).
  - Configured static linking options and MSVC-specific C++20 standard requirement for Box2D compilation.

### New Examples

- **Button Example (`examples/button`)**:
  - Demonstrates using `StaticBody` sensor shapes and mouse hover signals (`mouseEntered`, `mouseExited`) to build interactive UI buttons.
- **Multi-Window Example (`examples/multi-window`)**:
  - Demonstrates rendering content to multiple concurrent windows with separate root nodes and cameras.
- **Raycast Example (`examples/raycast`)**:
  - Showcases dynamic raycasting, rotation, and segment visualization.
- **Graphics Settings Example (`examples/graphics-settings`)**:
  - Demonstrates querying supported display modes, cycling modes, and switching fullscreen states at runtime.
