#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"

#ifdef PSP
// hello. i only added this here so that you can compile it to the psp but you are NOT supposed to run this example on the PSP!!!
// you are also NOT supposed to run this example on emscripten!!!
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

int main()
{
    Logger::init();
    Engine engine = Engine("Multi Window Example");
    Graphics *graphics = nullptr;

    auto window1 = std::make_shared<Window>("Window 1", Rect({100, 100}, {480, 272}));
    auto window2 = std::make_shared<Window>("Window 2", Rect({580, 100}, {480, 272}));
    engine.appendWindow(window1);
    engine.appendWindow(window2);
    graphics = new Graphics();
    Physics physics;
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // window 1
    auto cameraNode = std::make_shared<Node>();
    auto camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window1->root->addChild(cameraNode);

    auto node = std::make_shared<Node>();
    auto label = std::make_shared<Label>("Window 1");
    label->size = {200, 200};
    node->transform.position = {-240, -136};
    node->addComponent(label);
    window1->root->addChild(node);

    // window 2
    auto cameraNode2 = std::make_shared<Node>();
    auto camera2 = std::make_shared<Camera>();
    cameraNode2->addComponent(camera2);
    window2->root->addChild(cameraNode2);

    auto node2 = std::make_shared<Node>();
    auto label2 = std::make_shared<Label>("Window 2");
    label2->size = {200, 200};
    node2->transform.position = {-240, -136};
    node2->addComponent(label2);
    window2->root->addChild(node2);

    engine.initialize();

    while (!input.shouldQuit)
    {
        engine.update();
        engine.render(*graphics);
    }

    engine.exit();
}
