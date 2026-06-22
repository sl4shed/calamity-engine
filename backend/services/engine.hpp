#pragma once
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "graphics/definitions.hpp"
#include "services.hpp"

/**
 * # Engine
 * This class is mostly used to abstract stuff like the root node and obtaining deltaTime. It determines when the lifetime functions of every other node and component is called.
 *
 * The engine class should be initialized in the main function after nodes are setup and have its update and render functions called in the main loop. In the end, the `engine.exit()` function should be called.
 * Also, the engines constructor defines the app name which is used when reading/writing to the user folder. All Calamity Engine apps are stored under the Calamity Engine base folder and the app name is used as the organization folder. For more information [read this SDL3 wiki page](https://wiki.libsdl.org/SDL3/SDL_GetUserFolder).
 * 
 * You also must append atleast 1 Window to the engine for anything to render to the screen.
 * ```cpp
 * Graphics graphics = Graphics(); 
 * Engine engine = Engine("Your app name goes here");
 * auto window = std::make_shared<Window>("my window title", Rect({0, 0}, {480, 272}), RenderLogicalPresentation::LETTERBOX, WindowFlags::RESIZABLE, Color::BLACK);
 * engine.appendWindow(window);
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
    const std::string appName;

    int appendWindow(std::shared_ptr<Window> window);
    void removeWindow(int id);
    std::shared_ptr<Window> getWindow(int id);
    std::vector<std::shared_ptr<Window>> getWindows();

    void update();
    void render(Graphics &graphics);
    void initialize();
    void exit();

    float physicsTimestep  = 1.0f / 60.0f;
    float accumulator = 0.0f;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    float maxFps = 0.0f; /* Max frames per second the game is allowed to hit. 0.0f = unlimited */

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(windows));
    }

private:
    std::vector<std::shared_ptr<Window>> windows;
};