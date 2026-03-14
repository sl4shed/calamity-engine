#pragma once
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"

/**
 * This class is mostly used to abstract stuff like the root node and obtaining deltaTime.
 *
 * It should be initialized in the main function and have its update and render functions called in the main loop:
 * ```cpp
 * Graphics graphics = Graphics({480, 272}); // screen size vector2
 * Engine engine = Engine();
 * Services::init(&graphics, &engine, ...); // imagine every other core class in here aswell
 *
 * // setup nodes, components, etc here
 *
 * engine->initialize(); // VERY IMPORTANT
 * while (running) {
 *  engine.update();
 *  engine.render(Services::graphics());
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