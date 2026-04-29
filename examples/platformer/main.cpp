#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
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
static Engine engine = Engine("Platformer Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    graphics = new Graphics({480, 272}, "Platformer Example", RenderLogicalPresentation::LETTERBOX, Color(0x00b7ff));
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // input stuff

    inputRegistry.addAction("left", 0.2f);
    auto leftEvent = std::make_unique<InputEventControllerButton>();
    leftEvent->device = 0;
    leftEvent->button = ControllerButton::DPAD_LEFT;
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>();
    leftEventK->pressed = true;
    leftEventK->scancode = Keycode::A;
    inputRegistry.actionAddEvent("left", std::move(leftEventK));

    inputRegistry.addAction("right", 0.2f);
    auto rightEvent = std::make_unique<InputEventControllerButton>();
    rightEvent->device = 0;
    rightEvent->button = ControllerButton::DPAD_RIGHT;
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>();
    rightEventK->pressed = true;
    rightEventK->scancode = Keycode::D;
    inputRegistry.actionAddEvent("right", std::move(rightEventK));

    inputRegistry.addAction("up", 0.2f);
    auto upEvent = std::make_unique<InputEventControllerButton>();
    upEvent->device = 0;
    upEvent->button = ControllerButton::DPAD_UP;
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>();
    upEventK->pressed = true;
    upEventK->scancode = Keycode::W;
    inputRegistry.actionAddEvent("up", std::move(upEventK));

    // node stuff

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    cameraNode->transform.setScale({3, 3});
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);

    // player node
    std::shared_ptr<Node> node = std::make_shared<Node>("playerNode");
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://assets/cactus.png", TextureScaling::PIXELART);
    sprite->texture.width = 128;
    sprite->texture.height = 128;
    node->addComponent(sprite);
    auto shape = std::make_shared<BoxShape>(Vector2{128, 128});
    auto rbody = std::make_shared<RigidBody>(shape);
    rbody->fixRotation(true);
    node->addComponent(rbody);
    node->addComponent(std::make_shared<PlayerScript>());
    node->addChild(cameraNode); // parent the camera to the player so that it follows the player around :+1:

    // other nodes
    auto map = std::make_shared<Node>("map");
    map->transform.position = {0, 0};


    engine.root.addChild(map);
    engine.root.addChild(node);

    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit) {
        loop();
    }
    engine.shutdown();
    delete graphics;
#endif

    return 0;
}