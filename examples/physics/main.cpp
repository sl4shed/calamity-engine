#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/services/graphics/definitions.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/services/physics/definitions.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"
#include "backend/utils/utils.hpp"
#include "nodeScript.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Physics Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

static Physics physics = Physics({0, 9.81f});
static Engine engine = Engine("Physics Example");
static Graphics *graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main()
{
    Logger::init();

    auto window = std::make_shared<Window>("Physics Example", Rect{{0, 0}, {480, 272}});
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    ///////////////////////////////////////////

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

    /////////////////////////////////////

    auto cameraNode = std::make_shared<Node>("cameraNode");
    auto camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);

    auto node = std::make_shared<Node>("floorNode");
    node->transform.position = {0, 0};
    node->transform.setAngle(20.0f);
    auto shape = std::make_shared<BoxShape>(Vector2{300, 50});
    auto body = std::make_shared<StaticBody>(shape);
    body->collisionEnter.connect([body](PhysicsBody *b)
                                 { 
        Logger::info("Collision with platform!", b->getNode()->name);
        body->collisionEnter.reset(); });

    node->addComponent(body);
    node->addComponent(std::make_shared<ShapeSprite>(shape));
    window->root->addChild(cameraNode);
    window->root->addChild(node);

    std::shared_ptr<Node> labelNode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("Left Click/A - add object\nRight Click/B - clear objects");

    label->size = {200, 700};
    label->font->setSize(16);
    label->wrap = false;
    labelNode->transform.position = {-240, -136};
    labelNode->addComponent(label);
    window->root->addChild(labelNode);

    auto targetNode = std::make_shared<Node>("targetNode");
    auto objects = std::make_shared<Node>("objects");

    targetNode->addComponent(std::make_shared<NodeScript>());
    window->root->addChild(targetNode);
    window->root->addChild(objects);

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
