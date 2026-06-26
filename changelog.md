# Changelog

## [Alpha 1.1]

### New Features & Enhancements

- **Multi Window Support**: 
  - Restructured the entire graphics class to be able to handle multi-window rendering. Each window has it's own root node and camera! I did this in preparation for adding 3DS support (still not done yet, unfortunately).
  - Each window can be configured with its own dimensions, position, mode, clear color, and fullscreen state.
  - You can also dynamically change the fullscreen display mode just like in real video games!!! 😱
- **Tracy Profiler Support**:
  - You can now configure your Calamity Engine project with the option `-DTRACY_ENABLE=ON`. You can learn more about tracy over at it's [github repository](https://github.com/wolfpld/tracy).
- **Raycasts**:
  - You can now do raycasts with the `Raycast` class! There is also a new [raycast example](https://calamity.sl4shed.xyz/example-raycast)!
- **Collision Detection & Sensors**:
  - Added sensor support to shapes via `Shape::setSensor(bool)`, this disables collision but you can still recieve collision signals.
  - Added collision signals (`collisionEnter`, `collisionExit`, `collisionHit`) to every `PhysicsBody`.
- **Mouse Hover Signals for Physics Bodies**:
  - Added `mouseEntered` and `mouseExited` signals on every `PhysicsBody` to detect when the mouse enters or exits the collision shape.
- **New Physics Shapes**:
  - Added `SegmentShape`!
- **Node Visibility & State Control**:
  - Instead of the `visible` property depending on components (i.e. Sprites), now it's at the Node level. I added the `visible` property to the Node class which automatically applies to every component and child Node.
  - Also added `enabled` to the Node class which basically does the same thing as visible but instead it disables every lifetime function (`update`, `render`, `input`).
- **FPS Limiter**:
  - Added `maxFps` to the Engine service. By default it is set to 240 fps but can be changed to be unlimited.
- **Input Event Modifier Flags**:
  - Added parsing of modifier keys (`shiftPressed`, `altPressed`, `ctrlPressed`, `metaPressed`) in keyboard events. (which was overlooked in 1.0 but oh well it's here now)
- **Vector2 Utils**:
  - Added `lerp` and `distanceTo` functions to Vector2.
- **Windows support**
  - You can now use Calamity Engine on Windows!!! You can compile any Calamity Engine game with MSVC now. (as long as you don't use C style casts in your code... ask me how I know)
### Fixes & Refactoring

- **Got rid of SDL3_gfx and circle rendering is good now**:
  - Instead of using SDL3_gfx (which I don't even know why I was using in the first place), circle rendering is not a circle fan and it renders like a regular polygon. Looks much better! This also gets rid of a dependency which caused Windows builds to break.
- **Refactored Examples**:
  - I changed how the node-tree and physics examples work to be able to compile and work on console platforms. Try it out with a controller in your browser on the example website to see how it works!

### New Examples

- **Button Example (`examples/button`)**:
  - Demonstrates using `StaticBody` sensor shapes and mouse hover signals (`mouseEntered`, `mouseExited`) to create a working button (until I do the UI update).
- **Multi-Window Example (`examples/multi-window`)**:
  - Demonstrates rendering content to multiple concurrent windows with separate root nodes and cameras. This one can't compile to the browser, for obvious reasons.
- **Raycast Example (`examples/raycast`)**:
  - Showcases dynamic raycasting.
- **Graphics Settings Example (`examples/graphics-settings`)**:
  - Demonstrates querying supported display modes, cycling modes, and switching fullscreen states at runtime.
