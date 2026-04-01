#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"
#include "../core/ui/definitions.hpp"
#include "../core/ui/label.hpp"

enum class RenderLogicalPresentation
{
    DISABLED,  /**< There is no logical size in effect */
    STRETCH,   /**< The rendered content is stretched to the output resolution */
    LETTERBOX, /**< The rendered content is fit to the largest dimension and the other dimension is letterboxed with the clear color */
    OVERSCAN,  /**< The rendered content is fit to the smallest dimension and the other dimension extends beyond the output bounds */
    INTEGER_SCALE   /**< The rendered content is scaled up by integer multiples to fit the output resolution */
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
 *   engine.render(Services::graphics());
 * }
 * ```
 */
class Graphics
{
public:
    Graphics(Vector2 screenSize = {480, 272}, std::string windowTitle = "Calamity Engine", RenderLogicalPresentation presentation = RenderLogicalPresentation::LETTERBOX, Color clearColor = Color::WHITE);
    SDL_Texture *loadTexture(const std::string &path);
    void renderSprite(Node &node, Engine *engine);
    void renderLabel(Label *label);
    void preRender();
    void postRender();
    void resetLogicalPresentation();

    Vector2 screenSize = {480, 272};
    Color clearColor = Color::WHITE;
    RenderLogicalPresentation presentation = RenderLogicalPresentation::LETTERBOX;

    TTF_TextEngine *getTextEngine();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
};