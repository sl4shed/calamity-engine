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
#include "raycastScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Raycast Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Raycast Example");
static Graphics *graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main()
{
    Logger::init();

    auto window = std::make_shared<Window>("Raycast Example", Rect({0, 0}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

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

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://assets/cat.png", window);
    sprite->texture.width = 20;
    sprite->texture.height = 20;
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("A - rotate left\nD - rotate right");
    label->font->setSize(12);
    label->size = {200, 500};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    std::shared_ptr<Node> bnode = std::make_shared<Node>("floorNode");
    bnode->transform.position = {0, 130};
    auto shape = std::make_shared<BoxShape>(Vector2{300, 50});
    auto body = std::make_shared<StaticBody>(shape);
    bnode->addComponent(body);
    bnode->addComponent(std::make_shared<ShapeSprite>(shape));
    window->root->addChild(bnode);

    window->root->addChild(lnode);
    node->addComponent(std::make_shared<RaycastScript>());
    window->root->addChild(node);
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