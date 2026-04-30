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
#include "playerScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("Platformer Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics = Physics({0.0f, 9.81f});
static Engine engine = Engine("Platformer Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    graphics = new Graphics({480, 272}, "Platformer Example", RenderLogicalPresentation::LETTERBOX, Color(0x00b7ff));
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // input stuff

    inputRegistry.addAction("left", 0.2f);
    auto leftEvent = std::make_unique<InputEventControllerButton>();
    leftEvent->device = 0;
    leftEvent->button = ControllerButton::DPAD_LEFT;
    leftEvent->pressed = true;
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>();
    leftEventK->pressed = true;
    leftEventK->scancode = Keycode::A;
    inputRegistry.actionAddEvent("left", std::move(leftEventK));

    inputRegistry.addAction("right", 0.2f);
    auto rightEvent = std::make_unique<InputEventControllerButton>();
    rightEvent->device = 0;
    rightEvent->button = ControllerButton::DPAD_RIGHT;
    rightEvent->pressed = true;
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>();
    rightEventK->pressed = true;
    rightEventK->scancode = Keycode::D;
    inputRegistry.actionAddEvent("right", std::move(rightEventK));

    inputRegistry.addAction("up", 0.2f);
    auto upEvent = std::make_unique<InputEventControllerButton>();
    upEvent->device = 0;
    upEvent->button = ControllerButton::SOUTH;
    upEvent->pressed = true;
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>();
    upEventK->pressed = true;
    upEventK->scancode = Keycode::SPACE;
    inputRegistry.actionAddEvent("up", std::move(upEventK));

    // node stuff

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>("cameraNode");
    cameraNode->transform.setScale({2, 2});
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->smoothing = 0.3f;
    cameraNode->addComponent(camera);

    // player node
    auto node = std::make_shared<Node>("playerNode");
    node->transform.position = {0, -40};
    auto sprite = std::make_shared<AnimatedSprite>();

    // player animations
    auto idle = Animation("idle", 5, Vector2{14, 19} * 4, true, true);
    idle.textureScaling = TextureScaling::PIXELART;
    idle.texturePath = "res://assets/knight.png";
    idle.addFrames(
        Frame(Rect{{9, 9}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{41, 9}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{73, 9}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{105, 9}, {14, 19}}, {0.5, 0.5})
    );
    sprite->addAnimation(idle);

    auto run = Animation("run", 10, Vector2{14, 19} * 4, true, false);
    run.textureScaling = TextureScaling::PIXELART;
    run.texturePath = "res://assets/knight.png";
    run.addFrames(
        Frame(Rect{{8, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{41, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{73, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{104, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{136, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{169, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{200, 73}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{232, 73}, {14, 19}}, {0.5, 0.5}),

        Frame(Rect{{8, 104}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{41, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{72, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{104, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{136, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{168, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{200, 105}, {14, 19}}, {0.5, 0.5}),
        Frame(Rect{{232, 105}, {14, 19}}, {0.5, 0.5})
    );
    sprite->addAnimation(run);

    // rest of player node code
    node->addComponent(sprite);
    auto shape = std::make_shared<BoxShape>(Vector2{13, 19} * 4);
    Material mat = Material();
    mat.density = 10.0f;
    shape->applyMaterial(mat);
    auto rbody = std::make_shared<RigidBody>(shape);
    rbody->lockRotation(true);

    node->addComponent(rbody);
    node->addComponent(std::make_shared<PlayerScript>());
    node->addChild(cameraNode);

    // other nodes
    auto map = std::make_shared<Node>("map");
    map->transform.position = {0, 0};
    auto tilemap = std::make_shared<Tilemap>("res://assets/world_tileset.png", TextureScaling::PIXELART, Vector2{64, 64});
    // 896x192
    tilemap->addTiles(
        // grass
        Tile(Vector2{-10, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-9, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-8, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-7, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-6, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-5, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-4, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-3, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-2, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-1, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{0, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{1, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{2, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{3, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{4, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{5, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{6, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{7, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{8, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{9, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{10, 0}, Rect{{0, 0}, {16, 16}}),

        // dirt 1
        Tile(Vector2{-10, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-9, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-8, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-7, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-6, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-5, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-4, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-3, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-2, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-1, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{0, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{1, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{2, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{3, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{4, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{5, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{6, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{7, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{8, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{9, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{10, 1}, Rect{{0, 16}, {16, 16}}),

        // dirt 2
        Tile(Vector2{-10, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{-9, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{-8, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{-7, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{-6, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{-5, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{-4, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{-3, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{-2, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{-1, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{0, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{1, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{2, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{3, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{4, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{5, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{6, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{7, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{8, 2}, Rect{{16, 16}, {16, 16}}),
        Tile(Vector2{9, 2}, Rect{{16, 0}, {16, 16}}),
        Tile(Vector2{10, 2}, Rect{{16, 16}, {16, 16}}),

        // bridge
        Tile(Vector2{11, 0}, Rect{{144, 0}, {16, 16}}),
        Tile(Vector2{12, 0}, Rect{{161, 0}, {16, 16}}),
        Tile(Vector2{13, 0}, Rect{{161, 0}, {16, 16}}),
        Tile(Vector2{14, 0}, Rect{{176, 0}, {16, 16}}),

        // sandstone
        Tile(Vector2{15, 0}, Rect{{32, 0}, {16, 16}}),
        Tile(Vector2{16, 0}, Rect{{32, 0}, {16, 16}}),
        Tile(Vector2{17, 0}, Rect{{32, 0}, {16, 16}}),
        Tile(Vector2{15, 1}, Rect{{32, 16}, {16, 16}}),
        Tile(Vector2{16, 1}, Rect{{32, 16}, {16, 16}}),
        Tile(Vector2{17, 1}, Rect{{32, 16}, {16, 16}}),
        Tile(Vector2{15, 2}, Rect{{48, 0}, {16, 16}}),
        Tile(Vector2{16, 2}, Rect{{48, 16}, {16, 16}}),
        Tile(Vector2{17, 2}, Rect{{48, 0}, {16, 16}})
    );

    std::vector<Vector2> groundPoints;
    groundPoints.emplace_back(Vector2{-640, 0});
    groundPoints.emplace_back(Vector2{704, 0});
    groundPoints.emplace_back(Vector2{704, 192});
    groundPoints.emplace_back(Vector2{-640, 192});
    auto mshape = std::make_shared<PolygonShape>(groundPoints);
    map->addComponent(std::make_shared<StaticBody>(mshape));

    std::vector<Vector2> bridgePoints;
    bridgePoints.emplace_back(Vector2{704, 64});
    bridgePoints.emplace_back(Vector2{960, 64});
    bridgePoints.emplace_back(Vector2{960, 0});
    bridgePoints.emplace_back(Vector2{704, 0});
    auto bridgeShape = std::make_shared<PolygonShape>(bridgePoints);
    map->addComponent(std::make_shared<StaticBody>(bridgeShape));

    std::vector<Vector2> sandpoints;
    sandpoints.emplace_back(Vector2{960, 0});
    sandpoints.emplace_back(Vector2{1152, 0});
    sandpoints.emplace_back(Vector2{1152, 192});
    sandpoints.emplace_back(Vector2{960, 192});
    auto sandshape = std::make_shared<PolygonShape>(sandpoints);
    map->addComponent(std::make_shared<StaticBody>(sandshape));

    map->addComponent(tilemap);

    engine.root.addChild(map);
    engine.root.addChild(node);

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
