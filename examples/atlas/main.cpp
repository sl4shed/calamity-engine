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
#include "atlasScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Atlas Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Atlas Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    auto window = std::make_shared<Window>("Atlas Example", Rect({0, 0}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    inputRegistry.addAction("1", 0.2f);
    auto event1 = std::make_unique<InputEventControllerButton>();
    event1->device = 0;
    event1->pressed = true;
    event1->button = ControllerButton::LEFT_SHOULDER;
    inputRegistry.actionAddEvent("1", std::move(event1));

    auto event1k = std::make_unique<InputEventKey>();
    event1k->scancode = Keycode::W;
    event1k->pressed = true;
    inputRegistry.actionAddEvent("1", std::move(event1k));

    inputRegistry.addAction("2", 0.2f);
    auto event2 = std::make_unique<InputEventControllerButton>();
    event2->device = 0;
    event2->pressed = true;
    event2->button = ControllerButton::RIGHT_SHOULDER;
    inputRegistry.actionAddEvent("2", std::move(event2));

    auto event2k = std::make_unique<InputEventKey>();
    event2k->scancode = Keycode::A;
    event2k->pressed = true;
    inputRegistry.actionAddEvent("2", std::move(event2k));


    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://assets/atlas.png", window);
    sprite->sourceRect.size = {160, 160};
    sprite->sourceRect.position = {160, 0};
    sprite->texture.width = 160;
    sprite->texture.height = 160;
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("W\LSB - texture 1\nA\RSB - texture 2");
    label->font->setSize(12);
    label->size = {200, 500};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(lnode);
    node->addComponent(std::make_shared<AtlasScript>());
    window->root->addChild(node);
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