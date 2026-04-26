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

static Physics physics;
static Engine engine = Engine("Animated Sprite Example");
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

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    engine.root.addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    std::shared_ptr<AnimatedSprite> sprite = std::make_shared<AnimatedSprite>();
    Animation anim = Animation("test", 2, {128, 128}, true, true);
    anim.texturePath = "res://assets/frames.png";
    anim.addFrames(
        Frame({{0,   0}, {32, 32}}, {0.5, 0.5}, Color::BLUE),
        Frame({{32,  0}, {32, 32}}),
        Frame({{64,  0}, {32, 32}}),
        Frame({{64,  0}, {32, 32}}),
        Frame({{64,  0}, {32, 32}}),
        Frame({{64,  0}, {32, 32}}),
        Frame({{96,  0}, {32, 32}}),
        Frame({{128, 0}, {32, 32}})
    );
    sprite->addAnimation(anim);
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("Animated sprite example :)");
    label->font->setSize(12);
    label->size = {200, 500};
    label->screenSpace = true;
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    engine.root.addChild(lnode);
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