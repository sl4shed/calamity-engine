#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/services/physics/definitions.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"
#include "rootScript.hpp"

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
static Engine engine = Engine("Physics Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    graphics = new Graphics({480, 272});
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    inputRegistry.addAction("add", 0.2f);
    auto addEvent = std::make_unique<InputEventMouseButton>();
    addEvent->pressed = true;
    addEvent->buttonIndex = MouseButton::LEFT;
    inputRegistry.actionAddEvent("add", std::move(addEvent));

    auto ctrEvent = std::make_unique<InputEventControllerButton>();
    ctrEvent->pressed = true;
    ctrEvent->button = ControllerButton::SOUTH;
    ctrEvent->device = 0;
    inputRegistry.actionAddEvent("add", std::move(ctrEvent));

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>("cameraNode");
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);

    std::shared_ptr<Node> node = std::make_shared<Node>("floorNode");
    node->transform.position = {-150, 0};
    node->transform.setAngle(20.0f);
    auto shape = std::make_shared<BoxShape>(Vector2{300, 50});
    node->addComponent(std::make_shared<StaticBody>(shape));
    node->addComponent(std::make_shared<ShapeSprite>(shape));
    engine.root.addChild(cameraNode);
    engine.root.addChild(node);

    std::shared_ptr<Node> labelNode = std::make_shared<Node>();

#ifdef PSP
    std::shared_ptr<Label> label = std::make_shared<Label>("X - add box");
#else
    std::shared_ptr<Label> label = std::make_shared<Label>("left click - add box\nright click - clear boxes");
#endif

    label->size = {200, 700};
    label->font->setSize(16);
    label->wrap = false;
    labelNode->transform.position = {-240, -136};
    labelNode->addComponent(label);
    engine.root.addChild(labelNode);

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
