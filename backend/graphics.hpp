#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "definitions.hpp"

class Graphics {
    public:
        Graphics();
        Texture loadTexture(const std::string& path);
        void renderSprite(Transform rect, Texture texture, Transform source_rect = {});
        void preRender();
        void postRender();
        SDL_Window * window;
        SDL_Renderer * renderer;
};

#endif
