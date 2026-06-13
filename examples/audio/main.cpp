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
#include "audioScript.hpp"

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Audio Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("Calamity Engine");
static Graphics* graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    auto window = std::make_shared<Window>("Audio Example", Rect({0, 0}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();

    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    inputRegistry.addAction("play");
    auto event = std::make_unique<InputEventKey>();
    event->pressed = true;
    event->scancode = Keycode::SPACE;
    inputRegistry.actionAddEvent("play", std::move(event));

    auto eventC = std::make_unique<InputEventControllerButton>();
    eventC->device = 0;
    eventC->button = ControllerButton::SOUTH;
    eventC->pressed = true;
    inputRegistry.actionAddEvent("play", std::move(eventC));

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    node->addComponent(std::make_shared<Sprite>("res://assets/speaker.png", window));
    std::shared_ptr<AudioSource> sound = std::make_shared<AudioSource>("res://assets/sound.wav");
    node->transform.setScale({0.5f, 0.5f});
    sound->loop = true;
    node->addComponent(sound);
    node->addComponent(std::make_shared<AudioScript>());

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("Space/A - Play/Stop sound");
    label->font->setSize(12);
    label->size = {200, 500};
    lnode->transform.position = {-240, -136};
    lnode->addComponent(label);

    window->root->addChild(node);
    window->root->addChild(lnode);

    engine.initialize();

    // play sound
    sound->finished.connect([sound]() {
        Logger::info("Sound finished playing!");
        sound->finished.reset();
    });

    sound->looped.connect([sound]()
    {
        Logger::info("Sound looped!");
        sound->looped.reset();
    });

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while(!input.shouldQuit) {
        loop();
    }
#endif


    engine.exit();
}
