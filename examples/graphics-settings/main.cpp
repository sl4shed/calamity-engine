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

int main()
{
    Logger::init();

    Physics physics;
    Engine engine = Engine("Graphics Settings Example");
    Graphics graphics;
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    auto window = std::make_shared<Window>("Graphics Settings Example", Rect({100, 100}, {800, 600}));
    engine.appendWindow(window);

    Services::init(&graphics, &physics, &engine, &input, &inputRegistry, &audio);

    auto cameraNode = std::make_shared<Node>();
    auto camera = std::make_shared<Camera>();
    camera->origin = Vector2{0.0f, 0.0f};
    cameraNode->addComponent(camera);
    window->root->addChild(cameraNode);

    ////////////////////////////////////////////////////////////////////////////////////////////

    auto currentFullscreenMode = std::make_shared<Node>("fullscreenMode");
    currentFullscreenMode->transform.position = {300, 60};
    auto currentFullscreenModeLabel = std::make_shared<Label>("1920x1080*2@60");
    currentFullscreenModeLabel->origin = {0.5f, 0.5f};
    currentFullscreenMode->addComponent(currentFullscreenModeLabel);

    auto previousFullscreenMode = std::make_shared<Node>("previousFullscreenMode");
    previousFullscreenMode->transform.setScale({0.6f, 0.6f});
    previousFullscreenMode->transform.position = {300, 30};
    auto previousFullscreenModeLabel = std::make_shared<Label>("800x600*1000@60");
    previousFullscreenModeLabel->origin = {0.5f, 0.5f};
    previousFullscreenMode->addComponent(previousFullscreenModeLabel);

    auto nextFullscreenMode = std::make_shared<Node>("nextFullscreenMode");
    nextFullscreenMode->transform.setScale({0.6f, 0.6f});
    nextFullscreenMode->transform.position = {300, 90};
    auto nextFullscreenModeLabel = std::make_shared<Label>("4K*2@11000");
    nextFullscreenModeLabel->origin = {0.5f, 0.5f};
    nextFullscreenMode->addComponent(nextFullscreenModeLabel);

    window->root->addChild(currentFullscreenMode);
    window->root->addChild(previousFullscreenMode);
    window->root->addChild(nextFullscreenMode);

    //////////////////////////////////////////////////////////////////////////////////////////

    auto cycleNextFullscreenMode = std::make_shared<ButtonNode>("↑", "↑", Vector2{40.0f, 40.0f}, Vector2{0, 0});
    cycleNextFullscreenMode->getComponent<Label>()->font->setSize(30);
    cycleNextFullscreenMode->transform.position = {600, 10};

    auto cyclePreviousFullscreenMode = std::make_shared<ButtonNode>("↓", "↓", Vector2{40.0f, 40.0f}, Vector2{0, 0});
    cyclePreviousFullscreenMode->getComponent<Label>()->font->setSize(30);
    cyclePreviousFullscreenMode->transform.position = {600, 50};

    window->root->addChild(cyclePreviousFullscreenMode);
    window->root->addChild(cycleNextFullscreenMode);

    //////////////////////////////////////////////////////////////////////////////////////////

    auto fullscreenNode = std::make_shared<ButtonNode>("fullscreen", "Toggle Fullscreen", Vector2{100.0f, 50.0f}, Vector2{0, 0});
    fullscreenNode->transform.position = {20, 20};
    fullscreenNode->clicked.connect([fullscreenNode, window]()
                          { window->setFullscreen(!window->fullscreen); fullscreenNode->clicked.reset(); });

    window->root->addChild(fullscreenNode);
    engine.initialize();

    while (!input.shouldQuit)
    {
        engine.update();
        engine.render(graphics);
    }

    engine.exit();
    return 0;
}