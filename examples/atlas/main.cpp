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
#include "atlasScript.hpp"

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
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics({480, 272}, "Atlas Example", RenderLogicalPresentation::LETTERBOX, {0, 0, 0});
    InputRegistry inputRegistry = InputRegistry();
    Physics physics = Physics();
    Services::init(&graphics, &engine, &input, &inputRegistry, &physics);

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    engine.root.addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
    sprite->texture = Texture("assets/atlas.png");
    sprite->sourceTransform.scale({0.5, 1});
    sprite->sourceTransform.position = {160, 0};
    node->transform.scale(Vector2(0.5, 1) * 0.2);
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("W - texture 1\nA - texture 2");
    label->font->setSize(12);
    label->size = {200, 500};
    lnode->transform.position = {20, 20};
    lnode->addComponent(label);
    engine.root.addChild(lnode);

    node->addComponent(std::make_shared<AtlasScript>());
    engine.root.addChild(node);
    engine.initialize();

    while(!input.shouldQuit) {
        engine.update();
        engine.render(graphics);
    }

    engine.shutdown();
}
