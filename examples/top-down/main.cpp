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
static Engine engine = Engine("Top Down Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    graphics = new Graphics({480, 272}, "Top Down Example", RenderLogicalPresentation::LETTERBOX);
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
    rbody->lockRotation(true);
    node->addComponent(rbody);
    node->addComponent(std::make_shared<PlayerScript>());
    node->addChild(cameraNode); // parent the camera to the player so that it follows the player around :+1:

    // other nodes
    auto cat = std::make_shared<Node>("catNode");
    cat->transform.position = {200, 200};
    auto csprite = std::make_shared<Sprite>("res://assets/cat.png", TextureScaling::LINEAR);
    csprite->texture.width = 256;
    csprite->texture.height = 256;
    cat->addComponent(csprite);
    auto cshape = std::make_shared<BoxShape>(Vector2{256, 256}, Vector2{0.5f, 0.5f});
    cat->addComponent(std::make_shared<StaticBody>(cshape));

    auto hotdog = std::make_shared<Node>("hotdogNode");
    hotdog->transform.position = {-200, -250};
    auto hsprite = std::make_shared<Sprite>("res://assets/hotdog.png", TextureScaling::LINEAR);
    hotdog->addComponent(hsprite);
    auto hshape = std::make_shared<BoxShape>(Vector2{500, 340}, Vector2{0.5f, 0.5f});
    //hotdog->addComponent(std::make_shared<ShapeSprite>(hshape));
    hotdog->addComponent(std::make_shared<StaticBody>(hshape));

    engine.root.addChild(cat);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("WASD/Joystick - move player");
    label->font->setSize(12);
    label->size = {480, 200};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    engine.root.addChild(node);
    engine.root.addChild(hotdog);
    engine.root.addChild(lnode);

    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit) {
        loop();
    }
    engine.exit();
    delete graphics;
#endif

    return 0;
}