#pragma once
#include "definitions.hpp"
#include "node.hpp"

/**
 * This class is mostly used to abstract stuff like the root node and obtaining deltaTime.
 *
 * It should be initialized in the main function and have its update and render functions called in the main loop:
 * ```cpp
 * int main() {
 *  Graphics graphics = Graphics({480, 272}); // screen size vector2
 *  Engine engine = Engine();
 *
 *  // setup nodes, components, etc here
 *
 *  while (running) {
 *   engine.update();
 *   engine.render(graphics);
 *  }
 *  return 0;
 * }
 * ```
 *
 */
class Engine
{
public:
    Engine();
    ~Engine();
    Node root;
    void update();
    void render(Graphics &graphics);

    void setActiveCamera(Camera *camera);
    Camera *getActiveCamera();

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(root));
    }

private:
    Camera *activeCamera = nullptr;
};