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

    auto fullscreenSettingsNode = std::make_shared<Node>("fullscreenSettingsNode");
    fullscreenSettingsNode->visible = false;
    fullscreenSettingsNode->transform.position = {300, 20};

    ////////////////////////////////////////////////////////////////////////////////////////////

    auto currentFullscreenMode = std::make_shared<Node>("fullscreenMode");
    currentFullscreenMode->transform.position = {0, 40};
    auto currentFullscreenModeLabel = std::make_shared<Label>("1920x1080*2@60");
    currentFullscreenModeLabel->origin = {0.5f, 0.5f};
    currentFullscreenMode->addComponent(currentFullscreenModeLabel);

    auto previousFullscreenMode = std::make_shared<Node>("previousFullscreenMode");
    previousFullscreenMode->transform.setScale({0.6f, 0.6f});
    previousFullscreenMode->transform.position = {0, 10};
    auto previousFullscreenModeLabel = std::make_shared<Label>("800x600*1000@60");
    previousFullscreenModeLabel->origin = {0.5f, 0.5f};
    previousFullscreenMode->addComponent(previousFullscreenModeLabel);

    auto nextFullscreenMode = std::make_shared<Node>("nextFullscreenMode");
    nextFullscreenMode->transform.setScale({0.6f, 0.6f});
    nextFullscreenMode->transform.position = {0, 70};
    auto nextFullscreenModeLabel = std::make_shared<Label>("4K*2@11000");
    nextFullscreenModeLabel->origin = {0.5f, 0.5f};
    nextFullscreenMode->addComponent(nextFullscreenModeLabel);

    fullscreenSettingsNode->addChild(currentFullscreenMode);
    fullscreenSettingsNode->addChild(previousFullscreenMode);
    fullscreenSettingsNode->addChild(nextFullscreenMode);

    //////////////////////////////////////////////////////////////////////////////////////////

    auto cycleNextFullscreenMode = std::make_shared<ButtonNode>("↑", "↑", Vector2{40.0f, 40.0f}, Vector2{0, 0});
    cycleNextFullscreenMode->getChild("labelNode")->getComponent<Label>()->font->setSize(30);
    cycleNextFullscreenMode->transform.position = {200, 0};
    cycleNextFullscreenMode->clicked.connect([cycleNextFullscreenMode]()
                                             { Logger::info("NEXT"); cycleNextFullscreenMode->clicked.reset(); });

    auto cyclePreviousFullscreenMode = std::make_shared<ButtonNode>("↓", "↓", Vector2{40.0f, 40.0f}, Vector2{0, 0});
    cyclePreviousFullscreenMode->getChild("labelNode")->getComponent<Label>()->font->setSize(30);
    cyclePreviousFullscreenMode->transform.position = {200, 60};

    fullscreenSettingsNode->addChild(cyclePreviousFullscreenMode);
    fullscreenSettingsNode->addChild(cycleNextFullscreenMode);

    //////////////////////////////////////////////////////////////////////////////////////////

    window->root->addChild(fullscreenSettingsNode);

    /////////////////////////////////////////////////////////////////////////////////////////

    auto fullscreenNode = std::make_shared<ButtonNode>("fullscreen", "Toggle Fullscreen", Vector2{100.0f, 50.0f}, Vector2{0, 0});
    fullscreenNode->transform.position = {20, 20};
    fullscreenNode->clicked.connect([fullscreenNode, window, fullscreenSettingsNode]()
                                    { window->setFullscreen(!window->fullscreen); fullscreenSettingsNode->visible = !fullscreenSettingsNode->visible; fullscreenNode->clicked.reset(); });

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