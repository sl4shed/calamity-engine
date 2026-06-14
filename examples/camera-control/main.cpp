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

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main()
{
    Logger::init();

    auto window = std::make_shared<Window>("Camera Control Example", Rect({100, 100}, {480, 272}), RenderLogicalPresentation::LETTERBOX, WindowFlags::RESIZABLE, Color(0x5F5F5F));
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // input stuff

    inputRegistry.addAction("left", 0.2f);
    auto leftEvent = std::make_unique<InputEventControllerMotion>();
    leftEvent->device = 0;
    leftEvent->axis = ControllerAxis::LEFT_X;
    leftEvent->motion = -1.0f;
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>();
    leftEventK->pressed = true;
    leftEventK->scancode = Keycode::A;
    inputRegistry.actionAddEvent("left", std::move(leftEventK));

    inputRegistry.addAction("right", 0.2f);
    auto rightEvent = std::make_unique<InputEventControllerMotion>();
    rightEvent->device = 0;
    rightEvent->axis = ControllerAxis::LEFT_X;
    rightEvent->motion = 1.0f;
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>();
    rightEventK->pressed = true;
    rightEventK->scancode = Keycode::D;
    inputRegistry.actionAddEvent("right", std::move(rightEventK));

    inputRegistry.addAction("up", 0.2f);
    auto upEvent = std::make_unique<InputEventControllerMotion>();
    upEvent->device = 0;
    upEvent->axis = ControllerAxis::LEFT_Y;
    upEvent->motion = 1.0f;
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>();
    upEventK->pressed = true;
    upEventK->scancode = Keycode::W;
    inputRegistry.actionAddEvent("up", std::move(upEventK));

    inputRegistry.addAction("down", 0.2f);
    auto downEvent = std::make_unique<InputEventControllerMotion>();
    downEvent->device = 0;
    downEvent->axis = ControllerAxis::LEFT_Y;
    downEvent->motion = -1.0f;
    inputRegistry.actionAddEvent("down", std::move(downEvent));

    auto downEventK = std::make_unique<InputEventKey>();
    downEventK->pressed = true;
    downEventK->scancode = Keycode::S;
    inputRegistry.actionAddEvent("down", std::move(downEventK));

    // rotate inputs

    inputRegistry.addAction("rotateLeft", 0.2f);
    auto rotateLeftEvent = std::make_unique<InputEventControllerMotion>();
    rotateLeftEvent->device = 0;
    rotateLeftEvent->motion = 1.0f;
    rotateLeftEvent->axis = ControllerAxis::TRIGGER_LEFT;
    inputRegistry.actionAddEvent("rotateLeft", std::move(rotateLeftEvent));

    auto rotateLeftKeyEvent = std::make_unique<InputEventKey>();
    rotateLeftKeyEvent->pressed = true;
    rotateLeftKeyEvent->scancode = Keycode::Q;
    inputRegistry.actionAddEvent("rotateLeft", std::move(rotateLeftKeyEvent));

    inputRegistry.addAction("rotateRight", 0.2f);
    auto rotateRightEvent = std::make_unique<InputEventControllerMotion>();
    rotateRightEvent->device = 0;
    rotateRightEvent->motion = 1.0f;
    rotateRightEvent->axis = ControllerAxis::TRIGGER_RIGHT;
    inputRegistry.actionAddEvent("rotateRight", std::move(rotateRightEvent));

    auto rotateRightKeyEvent = std::make_unique<InputEventKey>();
    rotateRightKeyEvent->pressed = true;
    rotateRightKeyEvent->scancode = Keycode::E;
    inputRegistry.actionAddEvent("rotateRight", std::move(rotateRightKeyEvent));

    inputRegistry.addAction("rotateReset", 0.2f);
    auto rotateResetEvent = std::make_unique<InputEventControllerButton>();
    rotateResetEvent->device = 0;
    rotateResetEvent->button = ControllerButton::EAST;
    rotateResetEvent->pressed = true;
    inputRegistry.actionAddEvent("rotateReset", std::move(rotateResetEvent));

    auto rotateResetKeyEvent = std::make_unique<InputEventKey>();
    rotateResetKeyEvent->pressed = true;
    rotateResetKeyEvent->scancode = Keycode::R;
    inputRegistry.actionAddEvent("rotateReset", std::move(rotateResetKeyEvent));

    // node stuff

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://assets/grid.png", window);
    sprite->texture.width = 512;
    sprite->texture.height = 512;
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("WASD/Joystick - move camera\nQ/Left Trigger - rotate camera left\nE/Right Trigger - rotate camera right\nR/B - reset camera rotation");
    label->font->setSize(12);
    label->size = {480, 200};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(node);
    window->root->addChild(lnode);

    cameraNode->addComponent(std::make_shared<CameraScript>());
    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit)
    {
        loop();
    }
    engine.exit();
    delete graphics;
#endif

    return 0;
}