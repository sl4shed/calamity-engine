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
#include "playerScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("texture", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics = Physics({0.0f, 0.0f});
static Engine engine = Engine("Top Down Example");
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

    auto window = std::make_shared<Window>("Top Down Example", Rect({0, 0}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    input = new Input();
    inputRegistry = new InputRegistry();
    audio = new Audio();

    Services::init(graphics, &physics, &engine, input, inputRegistry, audio);

    // input stuff

    inputRegistry->addAction("left");
    auto leftEvent = std::make_unique<InputEventControllerMotion>(0, -1.0f, ControllerAxis::LEFT_X);
    inputRegistry->actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>(true, Keycode::A);
    inputRegistry->actionAddEvent("left", std::move(leftEventK));

    inputRegistry->addAction("right");
    auto rightEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::LEFT_X);
    inputRegistry->actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>(true, Keycode::D);
    inputRegistry->actionAddEvent("right", std::move(rightEventK));

    inputRegistry->addAction("up");
    auto upEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::LEFT_Y);
    inputRegistry->actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>(true, Keycode::W);
    inputRegistry->actionAddEvent("up", std::move(upEventK));

    inputRegistry->addAction("down");
    auto downEvent = std::make_unique<InputEventControllerMotion>(0, -1.0f, ControllerAxis::LEFT_Y);
    inputRegistry->actionAddEvent("down", std::move(downEvent));

    auto downEventK = std::make_unique<InputEventKey>(true, Keycode::S);
    inputRegistry->actionAddEvent("down", std::move(downEventK));

    // node stuff

    auto cameraNode = std::make_shared<Node>();
    cameraNode->transform.setScale({3, 3});
    auto camera = std::make_shared<Camera>();
    camera->smoothing = 0.1f;
    cameraNode->addComponent(camera);

    // player node
    auto node = std::make_shared<Node>("playerNode");
    auto sprite = std::make_shared<Sprite>("res://assets/cactus.png", window, TextureScaling::PIXELART);
    sprite->texture.width = 128;
    sprite->texture.height = 128;
    node->addComponent(sprite);
    auto shape = std::make_shared<BoxShape>(Vector2{128, 128});
    auto rbody = std::make_shared<RigidBody>(shape);
    rbody->lockRotation(true);
    node->addComponent(rbody);
    node->addComponent(std::make_shared<PlayerScript>());
    node->addChild(cameraNode); // parent the camera to the player so that it follows the player around :+1:

    // other nodes
    auto cat = std::make_shared<Node>("catNode");
    cat->transform.position = {200, 200};
    auto csprite = std::make_shared<Sprite>("res://assets/cat.png", window, TextureScaling::LINEAR);
    csprite->texture.width = 256;
    csprite->texture.height = 256;
    cat->addComponent(csprite);
    auto cshape = std::make_shared<BoxShape>(Vector2{256, 256}, Vector2{0.5f, 0.5f});
    cat->addComponent(std::make_shared<StaticBody>(cshape));

    auto hotdog = std::make_shared<Node>("hotdogNode");
    hotdog->transform.position = {-200, -250};
    auto hsprite = std::make_shared<Sprite>("res://assets/hotdog.png", window, TextureScaling::LINEAR);
    hotdog->addComponent(hsprite);
    auto hshape = std::make_shared<BoxShape>(Vector2{500, 340}, Vector2{0.5f, 0.5f});
    // hotdog->addComponent(std::make_shared<ShapeSprite>(hshape));
    hotdog->addComponent(std::make_shared<StaticBody>(hshape));

    window->root->addChild(cat);

    auto lnode = std::make_shared<Node>();
    auto label = std::make_shared<Label>("WASD/Joystick - Move player");
    label->font->setSize(12);
    label->size = {480, 200};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(node);
    window->root->addChild(hotdog);
    window->root->addChild(lnode);

    engine.initialize();

    return SDL_APP_CONTINUE;
}