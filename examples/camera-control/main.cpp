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
#include "cameraScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Camera Control Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Camera Control Example");
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

    auto window = std::make_shared<Window>("Camera Control Example", Rect({100, 100}, {480, 272}), RenderLogicalPresentation::LETTERBOX, WindowFlags::RESIZABLE, Color(0x5F5F5F));
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

    // rotate inputs

    inputRegistry->addAction("rotateLeft");
    auto rotateLeftEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::TRIGGER_LEFT);
    inputRegistry->actionAddEvent("rotateLeft", std::move(rotateLeftEvent));

    auto rotateLeftKeyEvent = std::make_unique<InputEventKey>(true, Keycode::Q);
    inputRegistry->actionAddEvent("rotateLeft", std::move(rotateLeftKeyEvent));

    inputRegistry->addAction("rotateRight");
    auto rotateRightEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::TRIGGER_RIGHT);
    inputRegistry->actionAddEvent("rotateRight", std::move(rotateRightEvent));

    auto rotateRightKeyEvent = std::make_unique<InputEventKey>(true, Keycode::E);
    inputRegistry->actionAddEvent("rotateRight", std::move(rotateRightKeyEvent));

    inputRegistry->addAction("rotateReset");
    auto rotateResetEvent = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::EAST);
    inputRegistry->actionAddEvent("rotateReset", std::move(rotateResetEvent));

    auto rotateResetKeyEvent = std::make_unique<InputEventKey>(true, Keycode::R);
    inputRegistry->actionAddEvent("rotateReset", std::move(rotateResetKeyEvent));

    // node stuff

    auto cameraNode = std::make_shared<Node>();
    auto camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    auto node = std::make_shared<Node>();
    auto sprite = std::make_shared<Sprite>("res://assets/grid.png", window);
    sprite->texture.width = 512;
    sprite->texture.height = 512;
    node->addComponent(sprite);

    auto lnode = std::make_shared<Node>();
    auto label = std::make_shared<Label>("WASD/Joystick - move camera\nQ/Left Trigger - rotate camera left\nE/Right Trigger - rotate camera right\nR/B - reset camera rotation");
    label->font->setSize(12);
    label->size = {480, 200};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(node);
    window->root->addChild(lnode);

    cameraNode->addComponent(std::make_shared<CameraScript>());
    engine.initialize();

    return SDL_APP_CONTINUE;
}