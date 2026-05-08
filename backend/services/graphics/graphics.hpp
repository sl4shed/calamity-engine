#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../../core/definitions.hpp"
#include "../../core/node/components.hpp"
#include "../../core/ui/definitions.hpp"
#include "../physics/definitions.hpp"
#include "definitions.hpp"

class Label; //forward declare

/**
 * # Graphics
 * The Graphics service is mostly made for internal use. It allows rendering sprites, labels, and other components on screen.
 * 
 * When constructing this class, the user can define the screen size, window title, presentation type and clear color:
 * ```cpp
 * // Create the graphics class with a 480x272 resolution, the window title of "my window title", letterbox presentation style and black clear color.
 * Graphics graphics = Graphics();
 * ```
 */
class Graphics
{
public:
    Graphics();
    SDL_Texture *loadTexture(const std::string &path, Window *window, TextureScaling scaling = TextureScaling::NEAREST) const;

    void exit();

    // renderComponent is a general function that renders different components based on different function overloads.
    void renderComponent(const Sprite &sprite, Window *window) const;
    void renderComponent(const ShapeSprite &sprite, Window *window) const;
    void renderComponent(const Label &label, Window *window) const;
    void renderComponent(const AnimatedSprite &sprite, Window *window) const;
    void renderComponent(const Tilemap &tilemap, Window *window) const;

    TTF_TextEngine *getTextEngine() const;
private:
    TTF_TextEngine *textEngine;
};