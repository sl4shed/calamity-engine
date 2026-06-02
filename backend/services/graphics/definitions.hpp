#pragma once
#include <string>
#include "../../core/definitions.hpp"
//#include "../../core/node/node.hpp"

class Node;
class Camera;
class Graphics;
class Engine;

enum class RenderLogicalPresentation
{
    DISABLED,     /**< There is no logical size in effect */
    STRETCH,      /**< The rendered content is stretched to the output resolution */
    LETTERBOX,    /**< The rendered content is fit to the largest dimension and the other dimension is letterboxed with the clear color */
    OVERSCAN,     /**< The rendered content is fit to the smallest dimension and the other dimension extends beyond the output bounds */
    INTEGER_SCALE /**< The rendered content is scaled up by integer multiples to fit the output resolution */
};

enum class WindowFlags
{
    FULLSCREEN = 0x0000000000000001,
    OCCLUDED = 0x0000000000000004,
    HIDDEN = 0x0000000000000008,
    BORDERLESS = 0x0000000000000010,
    RESIZABLE = 0x0000000000000020,
    MINIMIZED = 0x0000000000000040,
    MAXIMIZED = 0x0000000000000080,
    INPUT_GRABBED = 0x0000000000000100,
    INPUT_FOCUS = 0x0000000000000200,
    MOUSE_FOCUS = 0x0000000000000400,
    MODAL = 0x0000000000001000,
    HIGH_DPI = 0x0000000000002000,
    MOUSE_CAPTURE = 0x0000000000004000,
    ALWAYS_ON_TOP = 0x0000000000010000,
    UTILITY = 0x0000000000020000,
    TOOLTIP = 0x0000000000040000,
    POPUP_MENU = 0x0000000000080000,
    KEYBOARD_GRABBED = 0x0000000000100000,
    FILL_DOCUMENT = 0x0000000000200000, // Emscripten only
    TRANSPARENT = 0x0000000040000000
};

/**
 * # Window
 * The Window class represents a window on the screen. It contains a root node which is the parent of all nodes that are rendered to that window.
 *
 * When constructing a window, you can define the screen size, window title, presentation type, clear color and fullscreen mode:
 * ```cpp
 * auto window = std::make_shared<Window>("my window title", Rect({0, 0}, {480, 272}), RenderLogicalPresentation::LETTERBOX, WindowFlags::RESIZABLE, Color::BLACK, true);
 * ```
 *
 * Any window created must also be appended to the Engine class:
 * ```cpp
 * engine.appendWindow(window);
 * ```
 *
 * Afterwards, any nodes that are children of the window's root node will be rendered to the screen:
 * ```cpp
 * window->root->addChild(myNode);
 * ```
 */
class Window : public std::enable_shared_from_this<Window>
{
public:
    Window(std::string title = "Calamity App", Rect dimensions = Rect({0, 0}, {480, 272}), RenderLogicalPresentation presentation = RenderLogicalPresentation::LETTERBOX, WindowFlags flags = WindowFlags::RESIZABLE, Color clearColor = Color::BLACK, bool fullscreen = false);
    ~Window();

    std::string title;
    WindowFlags flags;
    RenderLogicalPresentation presentation;
    Rect dimensions;
    Color clearColor;
    std::unique_ptr<Node> root;
    bool fullscreen;
    int id = 0; // this is set when appending the window to the Engine service.

    void resetLogicalPresentation();

    void exit();
    void initialize();
    void render(Graphics &graphics, Engine *engine);
    void physicsUpdate();
    void update(float deltaTime);

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    void setActiveCamera(Camera *camera);
    Camera *getActiveCamera() const;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(title), CEREAL_NVP(flags), CEREAL_NVP(presentation), CEREAL_NVP(dimensions), CEREAL_NVP(fullscreen), CEREAL_NVP(id), CEREAL_NVP(root));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(title), CEREAL_NVP(flags), CEREAL_NVP(presentation), CEREAL_NVP(dimensions), CEREAL_NVP(fullscreen), CEREAL_NVP(id), CEREAL_NVP(root));

        for (auto &child : root->children)
        {
            child->setWindow(shared_from_this());
        }

        root->postLoad();
    }

private:
    Camera *activeCamera = nullptr;
    void preRender() const;
    void postRender() const;
};