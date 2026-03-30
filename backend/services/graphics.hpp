#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"
#include "../core/ui/definitions.hpp"
#include "../core/ui/label.hpp"

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
    Graphics(Vector2 screenSize = {480, 272}, std::string windowTitle = "Calamity Engine");
    SDL_Texture *loadTexture(const std::string &path);
    void renderSprite(Node &node, Engine *engine);
    void renderLabel(Label *label);
    void preRender();
    void postRender();
    Vector2 screenSize;

    TTF_TextEngine *getTextEngine();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
};