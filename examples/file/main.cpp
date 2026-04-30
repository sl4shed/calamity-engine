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
#include "backend/utils/file.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("File Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics;
static Engine engine = Engine("File Example");
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

    File *file = File::open("res://assets/label.txt", "r");
    std::string text = file->getAsText();
    file->close();

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>(text);
    label->font->setSize(20);
    label->font->setAlignment(FontAlignment::CENTER);
    label->size = {400, 272};
    label->origin = {0.5f, 0.5f};
    label->screenSpace = true;
    lnode->transform.position = {0, 0};
    lnode->addComponent(label);

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