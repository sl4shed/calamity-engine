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

static Physics physics;
static Engine engine = Engine("Multi Window Example");
static Graphics* graphics = nullptr;

void loop()
{
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    auto window1 = std::make_shared<Window>("Window 1", Rect({0, 0}, {480, 272}));
    auto window2 = std::make_shared<Window>("Window 2", Rect({480, 0}, {480, 272}));
    engine.appendWindow(window1);
    engine.appendWindow(window2);
    graphics = new Graphics();
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // window 1
    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    window1->root->addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("Window 1");
    label->size = {100, 500};
    node->transform.position = {-240, -136};
    node->addComponent(label);
    window1->root->addChild(node);

    // window 2
    std::shared_ptr<Node> cameraNode2 = std::make_shared<Node>();
    std::shared_ptr<Camera> camera2 = std::make_shared<Camera>();
    cameraNode2->addComponent(camera2);
    window2->root->addChild(cameraNode2);

    std::shared_ptr<Node> node2 = std::make_shared<Node>();
    std::shared_ptr<Label> label2 = std::make_shared<Label>("Window 2");
    label2->size = {100, 500};
    node2->transform.position = {240, -136};
    node2->addComponent(label2);
    window2->root->addChild(node2);

    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while(!input.shouldQuit) {
        loop();
    }
#endif

    engine.exit();
}
