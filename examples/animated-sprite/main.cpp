#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/services/graphics/definitions.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/ui/label.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Animated Sprite Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Animated Sprite Example");
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

    auto window = std::make_shared<Window>("Animated Sprite Example", Rect({100, 100}, {480, 272}));
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

    auto node = std::make_shared<Node>();
    auto sprite = std::make_shared<AnimatedSprite>(window);
    Animation anim = Animation("test", 2, {128, 128}, true, true);
    anim.textureScaling = TextureScaling::PIXELART;
    anim.texturePath = "res://assets/frames.png";
    anim.addFrames(
        Frame({{0, 0}, {32, 32}}, {0.5, 0.5}, Color::BLUE),
        Frame({{32, 0}, {32, 32}}),
        Frame({{64, 0}, {32, 32}}),
        Frame({{64, 0}, {32, 32}}),
        Frame({{64, 0}, {32, 32}}),
        Frame({{64, 0}, {32, 32}}),
        Frame({{96, 0}, {32, 32}}),
        Frame({{128, 0}, {32, 32}}));
    sprite->addAnimation(anim);
    node->addComponent(sprite);

    auto lnode = std::make_shared<Node>();
    auto label = std::make_shared<Label>("Animated sprite example :)");
    label->font->setSize(12);
    label->size = {200, 500};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(lnode);
    window->root->addChild(node);
    engine.initialize();

    return SDL_APP_CONTINUE;
}