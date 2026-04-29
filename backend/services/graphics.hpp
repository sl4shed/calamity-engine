#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"
#include "../core/ui/definitions.hpp"
#include "physics/definitions.hpp"

class Label; //forward declare

enum class RenderLogicalPresentation
{
    DISABLED,  /**< There is no logical size in effect */
    STRETCH,   /**< The rendered content is stretched to the output resolution */
    LETTERBOX, /**< The rendered content is fit to the largest dimension and the other dimension is letterboxed with the clear color */
    OVERSCAN,  /**< The rendered content is fit to the smallest dimension and the other dimension extends beyond the output bounds */
    INTEGER_SCALE   /**< The rendered content is scaled up by integer multiples to fit the output resolution */
};

enum class WindowFlags {
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
 * Here is your modular graphics class, which you should technically be able to change out for another implementation
 * with another graphics API (OpenGL, for example). Right now it uses SDL3.
 *
 * This is typically initialized in the main function, and passed to the Services class, like this:
 * ```cpp
 * Graphics graphics = Graphics({480, 272}); // screen size vector2 (PSP screen size)
 * Engine engine = Engine();
 * Services::init(&graphics, &engine, ...); // imagine every other core class in here aswell
 *
 * // main loop (usually in a function for emscripten compatibility)
 * while (running) {
 *   engine.update();
 *   engine.render(graphics);
 * }
 * ```
 */
class Graphics
{
public:
    Graphics(Vector2 screenSize = {480, 272}, const std::string& title = "Calamity App", RenderLogicalPresentation presentation = RenderLogicalPresentation::LETTERBOX, Color clearColor = Color::BLACK, WindowFlags _flags = WindowFlags::RESIZABLE);
    SDL_Texture *loadTexture(const std::string &path, TextureScaling scaling = TextureScaling::NEAREST) const;

    // renderComponent is a general function that renders different components based on different function overloads.
    void renderComponent(const Sprite &sprite) const;
    void renderComponent(const ShapeSprite &sprite) const;
    void renderComponent(const Label &label) const;
    void renderComponent(const AnimatedSprite &sprite) const;
    void renderComponent(const Tilemap &tilemap) const;
    void preRender() const;
    void postRender() const;
    void resetLogicalPresentation();

    SDL_Renderer *getRenderer() const;
    const Vector2 screenSize;
    Color clearColor = Color::WHITE;
    RenderLogicalPresentation presentation = RenderLogicalPresentation::LETTERBOX;

    TTF_TextEngine *getTextEngine() const;
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
};