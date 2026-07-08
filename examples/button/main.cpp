#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/definitions.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/ui/label.hpp"
#include "button.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Atlas Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Button Example");
static Graphics *graphics = nullptr;
static Input *input = nullptr;
static InputRegistry *inputRegistry = nullptr;
static Audio *audio = nullptr;

SDL_AppResult SDL_AppIterate(void *appstate)
{
    engine.update();
    engine.render(*graphics);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    engine.exit();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return input->processInput(event);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    Logger::init();

    auto window = std::make_shared<Window>("Button Example", Rect({100, 100}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    input = new Input();
    inputRegistry = new InputRegistry();
    audio = new Audio();

    Services::init(graphics, &physics, &engine, input, inputRegistry, audio);

    auto cameraNode = std::make_shared<Node>();
    auto camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    auto node = std::make_shared<ButtonNode>("button1", "Click Me!");
    node->clicked.connect([node]()
                          { Logger::info("Button clicked!!!"); node->clicked.reset(); });

    window->root->addChild(node);
    engine.initialize();

    return SDL_APP_CONTINUE;
}