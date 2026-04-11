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

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("texture", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

int main() {
    Logger::init();
    Physics physics = Physics();
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics({480, 272}, "Sound Example", RenderLogicalPresentation::LETTERBOX, {0, 0, 0});
    InputRegistry inputRegistry = InputRegistry();
    Services::init(&graphics, &engine, &input, &inputRegistry, &physics);

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    engine.root.addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    std::shared_ptr<AudioSource> sound = std::make_shared<AudioSource>("assets/sound.wav");
    node->addComponent(sound);

    // play sound
    sound->playing = true;

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("The sound should start automatically.");
    label->font->setSize(12);
    label->size = {200, 500};
    lnode->transform.position = {20, 20};
    lnode->addComponent(label);
    engine.root.addChild(lnode);

    engine.root.addChild(node);
    engine.initialize();

    while(!input.shouldQuit) {
        engine.update();
        engine.render(graphics);
    }

    engine.shutdown();
}
