#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/definitions.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"
#include "boxScript.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Node Tree Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

static Physics physics = Physics({0, 9.81f});
static Engine engine = Engine("Node Tree Example");
static Graphics *graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main()
{
    Logger::init();

    auto window = std::make_shared<Window>("Node Tree Example", Rect({100, 100}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // Action registry stuff //

    inputRegistry.addAction("left");
    auto leftEvent = std::make_unique<InputEventControllerMotion>(0, -1.0f, ControllerAxis::LEFT_X);
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    inputRegistry.addAction("right");
    auto rightEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::LEFT_X);
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    inputRegistry.addAction("up");
    auto upEvent = std::make_unique<InputEventControllerMotion>(0, 1.0f, ControllerAxis::LEFT_Y);
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    inputRegistry.addAction("down");
    auto downEvent = std::make_unique<InputEventControllerMotion>(0, -1.0f, ControllerAxis::LEFT_Y);
    inputRegistry.actionAddEvent("down", std::move(downEvent));

    ///////////////////////// divider /////////////////////////////

    inputRegistry.addAction("add");
    auto addEvent = std::make_unique<InputEventMouseButton>(true, MouseButton::LEFT);
    inputRegistry.actionAddEvent("add", std::move(addEvent));

    auto addEventC = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::SOUTH);
    inputRegistry.actionAddEvent("add", std::move(addEventC));

    inputRegistry.addAction("clear");
    auto clearEvent = std::make_unique<InputEventMouseButton>(true, MouseButton::RIGHT);
    inputRegistry.actionAddEvent("clear", std::move(clearEvent));

    auto clearEventC = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::EAST);
    inputRegistry.actionAddEvent("clear", std::move(clearEventC));

    // Save/load actions

    inputRegistry.addAction("save");
    auto saveEvent = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::LEFT_SHOULDER);
    inputRegistry.actionAddEvent("save", std::move(saveEvent));

    auto saveEventK = std::make_unique<InputEventKey>(true, Keycode::Z);
    inputRegistry.actionAddEvent("save", std::move(saveEventK));

    inputRegistry.addAction("load");
    auto loadEvent = std::make_unique<InputEventControllerButton>(0, true, ControllerButton::RIGHT_SHOULDER);
    inputRegistry.actionAddEvent("load", std::move(loadEvent));

    auto loadEventK = std::make_unique<InputEventKey>(true, Keycode::V);
    inputRegistry.actionAddEvent("load", std::move(loadEventK));

    /////// Node setup /////////

    auto cameraNode = std::make_shared<Node>("cameraNode");
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->transform.position = {0, -100};
    cameraNode->transform.scale({3, 3});
    cameraNode->addComponent(camera);

    // box thing
    auto node2 = std::make_shared<Node>("floorNode2");
    node2->transform.position = {-200, -300};
    auto shape2 = std::make_shared<BoxShape>(Vector2{50, 350}, Vector2{1.0f, 1.0f});
    node2->addComponent(std::make_shared<StaticBody>(shape2));
    node2->addComponent(std::make_shared<ShapeSprite>(shape2));

    auto node3 = std::make_shared<Node>("floorNode3");
    node3->transform.position = {150, -300};
    auto shape3 = std::make_shared<BoxShape>(Vector2{50, 350}, Vector2{1.0f, 1.0f});
    node3->addComponent(std::make_shared<StaticBody>(shape3));
    node3->addComponent(std::make_shared<ShapeSprite>(shape3));

    auto node = std::make_shared<Node>("floorNode");
    node->transform.position = {-150, 0};
    auto shape = std::make_shared<BoxShape>(Vector2{300, 50}, Vector2{1.0f, 1.0f});
    node->addComponent(std::make_shared<StaticBody>(shape));
    node->addComponent(std::make_shared<ShapeSprite>(shape));

    window->root->addChild(cameraNode);
    window->root->addChild(node2);
    window->root->addChild(node3);
    window->root->addChild(node);

    auto labelNode = std::make_shared<Node>();
    auto label = std::make_shared<Label>("Mouse/Joystick - Move around box\nLeft Click/A - Add box\nRight Click/B - Clear boxes\nZ/LSB - Save\nV/RSB - Load");
    label->size = {200, 700};
    label->font->setSize(16);
    label->wrap = false;
    labelNode->transform.position = {-240, -136};
    labelNode->addComponent(label);
    label->screenSpace = true;
    window->root->addChild(labelNode);

    auto boxNode = std::make_shared<Node>("boxNode");
    auto sprite = std::make_shared<Sprite>("res://assets/cat.png", window);
    sprite->modulate = Color(255, 255, 255, 100);
    boxNode->addComponent(sprite);
    boxNode->addComponent(std::make_shared<BoxScript>());

    // catNode is where all of the boxes are spawned.
    // it makes it easier to clear all children (for example)
    // it's also just good practice generally.
    auto catNode = std::make_shared<Node>("catNode");
    window->root->addChild(catNode);
    window->root->addChild(boxNode);

    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit)
    {
        loop();
    }
#endif

    engine.exit();
}
