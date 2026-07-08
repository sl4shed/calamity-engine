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
#include "atlasScript.hpp"

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
static Engine engine = Engine("Atlas Example");
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

    auto window = std::make_shared<Window>("Atlas Example", Rect({100, 100}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    input = new Input();
    inputRegistry = new InputRegistry();
    audio = new Audio();

    Services::init(graphics, &physics, &engine, input, inputRegistry, audio);

    inputRegistry->addAction("1");
    auto event1 = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::LEFT_SHOULDER);
    inputRegistry->actionAddEvent("1", std::move(event1));

    auto event1k = std::make_unique<InputEventKey>(true, Keycode::W);
    inputRegistry->actionAddEvent("1", std::move(event1k));

    inputRegistry->addAction("2");
    auto event2 = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::RIGHT_SHOULDER);
    inputRegistry->actionAddEvent("2", std::move(event2));

    auto event2k = std::make_unique<InputEventKey>(true, Keycode::A);
    inputRegistry->actionAddEvent("2", std::move(event2k));

    auto cameraNode = std::make_shared<Node>();
    auto camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    auto node = std::make_shared<Node>();
    auto sprite = std::make_shared<Sprite>("res://assets/atlas.png", window);
    sprite->sourceRect.size = {160, 160};
    sprite->sourceRect.position = {160, 0};
    sprite->texture.width = 160;
    sprite->texture.height = 160;
    node->addComponent(sprite);

    auto lnode = std::make_shared<Node>();
    auto label = std::make_shared<Label>("W/LSB - texture 1\nA/RSB - texture 2");
    label->font->setSize(12);
    label->size = {200, 500};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(lnode);
    node->addComponent(std::make_shared<AtlasScript>());
    window->root->addChild(node);
    engine.initialize();

    return SDL_APP_CONTINUE;
}