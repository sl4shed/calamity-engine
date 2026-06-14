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
#include "button.hpp"

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
static Engine engine = Engine("Button Example");
static Graphics *graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main()
{
    Logger::init();

    auto window = std::make_shared<Window>("Button Example", Rect({100, 100}, {480, 272}));
    engine.appendWindow(window);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    auto node = std::make_shared<ButtonNode>("button1", "Click Me!");
    node->clicked.connect([node]()
                          { Logger::info("Button clicked!!!"); node->clicked.reset(); });

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