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
        this->setWrapWidth(static_cast<int>(size.x));
    }

    rebuildTexture();
}

SDL_Texture *Label::getTexture() const
{
    return texture;
}

void Label::rebuildTexture() {
    if(!dirty) return;
    dirty = false;

    if(texture)
        SDL_DestroyTexture(texture);

    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font->getHandle(), text.c_str(), text.size(), color, wrap ? static_cast<int>(size.x) : 0);
    texture = SDL_CreateTextureFromSurface(Services::graphics()->getRenderer(), surface);
    SDL_DestroySurface(surface);
}

std::string Label::getText() const {
    return text;
}

TTF_Text* Label::getHandle() const {
    return handle;
}

Color Label::getColor() const
{
    Uint8 r, g, b, a;
    TTF_GetTextColor(handle, &r, &g, &b, &a);
    return {r, g, b, a};
}

Label::Label(const std::string& text, Font *font) {
    this->font = font;
    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
}

Label::Label(const std::string& text) {
#ifdef EMSCRIPTEN
    static auto dfont = Font("/calamity/default.ttf");
#else
    static auto dfont = Font("./calamity/default.ttf");
#endif
    this->font = &dfont;

    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
    this->setColor({255, 255, 255, 255});
}

Label::~Label() {
    TTF_DestroyText(handle);
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
    TTF_SetTextDirection(handle, static_cast<TTF_Direction>(direction));
    return this;
}

Label* Label::setText(const std::string& _text) {
    this->text = _text;
    dirty = true;
    TTF_SetTextString(handle, text.c_str(), text.size());
    return this;
}

Label* Label::setWrapWidth(const int width) {
    this->wrapWidth = width;
    dirty = true;
    TTF_SetTextWrapWidth(handle, wrapWidth);
    return this;
}

int Label::getWrapWidth() const
{
    return wrapWidth;
}
