#include "definitions.hpp"
#include "label.hpp"
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../services/graphics.hpp"
#include "../../utils/logger.hpp"
#include "../node/components.hpp"
#include "../node/node.hpp"

void Label::update(float dt) {
    if(size != prevSize) {
        prevSize = size;
        this->setWrapWidth(size.x);
    }

    rebuildTexture();
}

SDL_Texture *Label::getTexture() {
    return texture;
}

void Label::rebuildTexture() {
    if(!dirty) return;
    dirty = false;

    if(texture)
        SDL_DestroyTexture(texture);

    SDL_Surface *surface;
    // Pass size.x if wrapping, otherwise pass 0 to allow \n without horizontal wrapping
    surface = TTF_RenderText_Blended_Wrapped(font->getHandle(), text.c_str(), text.size(), color, wrap ? size.x : 0);

    texture = SDL_CreateTextureFromSurface(Services::graphics()->getRenderer(), surface);
    SDL_DestroySurface(surface);
}

std::string Label::getText() const {
    return text;
}

TTF_Text* Label::getHandle() const {
    return handle;
}

Color Label::getColor() {
    Uint8 r, g, b, a;
    TTF_GetTextColor(handle, &r, &g, &b, &a);
    return Color((int)r, (int)g, (int)b, (int)a);
}

Label::Label(std::string text, Font *font) {
    this->font = font;
    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
}

Label::Label(std::string text) {
#ifdef EMSCRIPTEN
    static Font dfont = Font("/calamity/default.ttf");
#else
    static Font dfont = Font("./calamity/default.ttf");
#endif
    this->font = &dfont;

    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
    this->setColor({255, 255, 255, 255});
}

Label::~Label() {
    TTF_DestroyText((TTF_Text*)handle);
}

Label* Label::setColor(Color _color) {
    this->color = _color;
    dirty = true;
    TTF_SetTextColor(handle, color.r, color.g, color.b, color.a);
    return this;
}

Label* Label::setDirection(FontDirection _direction) {
    this->direction = _direction;
    dirty = true;
    TTF_SetTextDirection(handle, (TTF_Direction)direction);
    return this;
}

Label* Label::setText(std::string _text) {
    this->text = _text;
    dirty = true;
    TTF_SetTextString(handle, text.c_str(), text.size());
    return this;
}

Label* Label::setWrapWidth(int width) {
    this->wrapWidth = width;
    dirty = true;
    TTF_SetTextWrapWidth(handle, wrapWidth);
    return this;
}

int Label::getWrapWidth() {
    return wrapWidth;
}