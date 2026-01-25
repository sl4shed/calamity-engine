#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "definitions.hpp"
#include "components.hpp"

/**
 * Here is your modular graphics class, which you should technically be able to change out for another implementation
 * with another graphics API (OpenGL, for example). Right now it uses SDL2.
 *
 * This is typically initialized in the main function, and passed to the Engine's render function, like this:
 * ```cpp
 * int main() {
 *   Graphics graphics = Graphics({480, 272}); // screen size vector2
 *   Engine engine = Engine();
 *
 *   // setup nodes, components, etc here
 *
 *   while (running) {
 *     engine.update();
 *     engine.render(graphics);
 *   }
 *   return 0;
 * }
 * ```
 *
 */
class Graphics
{
public:
    Graphics(Vector2 screenSize = {480, 272});
    Texture loadTexture(const std::string &path);
    void renderSprite(Node &node, Engine *engine);
    void preRender();
    void postRender();
    Vector2 screenSize;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};