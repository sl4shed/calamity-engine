#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"
#include "rootScript.hpp"
#include "fpsScript.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("texture", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

static Physics physics = Physics({0, 9.81f});
static Engine engine = Engine("Node Tree Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    graphics = new Graphics({480, 272}, "Node Tree Example");
    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    inputRegistry.addAction("add", 0.2f);
    auto addEvent = std::make_unique<InputEventMouseButton>();
    addEvent->pressed = true;
    addEvent->buttonIndex = MouseButton::LEFT;
    inputRegistry.actionAddEvent("add", std::move(addEvent));

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>("cameraNode");
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->transform.scale({3, 3});
    cameraNode->addComponent(camera);

    // box thing
    std::shared_ptr<Node> node2 = std::make_shared<Node>("floorNode2");
    node2->transform.position = {-200, -300};
    auto shape2 = std::make_shared<BoxShape>(Vector2{50, 350});
    node2->addComponent(std::make_shared<StaticBody>(shape2));
    node2->addComponent(std::make_shared<ShapeSprite>(shape2->polygon));

    std::shared_ptr<Node> node3 = std::make_shared<Node>("floorNode3");
    node3->transform.position = {150, -300};
    auto shape3 = std::make_shared<BoxShape>(Vector2{50, 350});
    node3->addComponent(std::make_shared<StaticBody>(shape3));
    node3->addComponent(std::make_shared<ShapeSprite>(shape3->polygon));

    std::shared_ptr<Node> node = std::make_shared<Node>("floorNode");
    node->transform.position = {-150, 0};
    auto shape = std::make_shared<BoxShape>(Vector2{300, 50});
    node->addComponent(std::make_shared<StaticBody>(shape));
    node->addComponent(std::make_shared<ShapeSprite>(shape->polygon));

    engine.root.addChild(cameraNode);
    engine.root.addChild(node2);
    engine.root.addChild(node3);
    engine.root.addChild(node);

    std::shared_ptr<Node> labelNode = std::make_shared<Node>();

#ifdef PSP
    std::shared_ptr<Label> label = std::make_shared<Label>("X - add box");
#else
    std::shared_ptr<Label> label = std::make_shared<Label>("Left Click - add box\nRight Click - clear boxes\nZ - save boxes\nV - load boxes");
#endif

    label->size = {200, 700};
    label->font->setSize(16);
    label->wrap = false;
    labelNode->transform.position = {-240, -136};
    labelNode->addComponent(label);
    label->screenSpace = true;
    engine.root.addChild(labelNode);

    std::shared_ptr<Node> catNode = std::make_shared<Node>("catNode");
    engine.root.addChild(catNode);

    std::shared_ptr<Node> fpsNode = std::make_shared<Node>("fpsNode");
    std::shared_ptr<Label> fpsLabel = std::make_shared<Label>("");
    fpsLabel->font->setSize(16);
    fpsLabel->screenSpace = true;
    fpsNode->transform.position = {-240, -60};
    fpsNode->addComponent(fpsLabel);
    fpsNode->addComponent(std::make_shared<FpsScript>());
    engine.root.addChild(fpsNode);

    engine.root.addComponent(std::make_shared<RootScript>());
    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while(!input.shouldQuit) {
        loop();
    }
#endif

    engine.shutdown();
}
