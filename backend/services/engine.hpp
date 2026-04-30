#pragma once
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "services.hpp"

/**
 * # Engine
 * This class is mostly used to abstract stuff like the root node and obtaining deltaTime. It determines when the lifetime functions of every other node and component is called.
 *
 * The engine class should be initialized in the main function after nodes are setup and have its update and render functions called in the main loop. In the end, the `engine.exit()` function should be called.
 * Also, the engines constructor defines the app name which is used when reading/writing to the user folder. All Calamity Engine apps are stored under the Calamity Engine base folder and the app name is used as the organization folder. For more information [read this SDL3 wiki page](https://wiki.libsdl.org/SDL3/SDL_GetUserFolder).
 * 
 * ```cpp
 * Graphics graphics = Graphics({480, 272}); // screen size vector2
 * Engine engine = Engine("Your app name goes here");
 * Services::init(&graphics, &engine, ...); // imagine every other core service in here aswell
 *
 * // setup nodes, components, etc here
 *
 * engine->initialize();
 * while (!input.shouldQuit) {
 *  engine.update();
 *  engine.render(Services::graphics());
 * }
 *
 * engine.exit();
 * ```
 *
 */
class Engine
{
public:
    Engine(std::string _appName = "Calamity App");
    ~Engine();

    Node root;
    const std::string appName;

    void update();
    void render(Graphics &graphics);
    void initialize();
    void exit();

    void setActiveCamera(Camera *camera);
    Camera *getActiveCamera() const;

    float physicsTimestep  = 1.0f / 60.0f;
    float accumulator = 0.0f;
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