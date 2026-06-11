#include "definitions.hpp"
#include "label.hpp"
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../services/graphics/graphics.hpp"
#include "../../utils/logger.hpp"
#include "../node/components.hpp"
#include "../node/node.hpp"

void Label::update(float dt)
{
    if (size != prevSize)
    {
        prevSize = size;
        this->setWrapWidth(static_cast<int>(size.x));
    }

    rebuildTexture();
}

SDL_Texture *Label::getTexture() const
{
    return texture;
}

void Label::rebuildTexture()
{
    if (!dirty)
        return;
    if (!getNode() || !getNode()->getWindow())
        return;
    dirty = false;

    if (texture)
        SDL_DestroyTexture(texture);

    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font->getHandle(), text.c_str(), text.size(), color, wrap ? static_cast<int>(size.x) : 0);
    texture = SDL_CreateTextureFromSurface(getNode()->getWindow()->renderer, surface);
    SDL_DestroySurface(surface);
}

std::string Label::getText() const
{
    return text;
}

TTF_Text *Label::getHandle() const
{
    return handle;
}

Color Label::getColor() const
{
    Uint8 r, g, b, a;
    TTF_GetTextColor(handle, &r, &g, &b, &a);
    return {r, g, b, a};
}

Label::Label() {
    static auto dfont = std::shared_ptr<Font>(new Font("res://calamity/default.ttf"));
    this->font = dfont;
    this->text = "";

    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
}

Label::Label(const std::string &text, std::shared_ptr<Font> font)
{
    this->font = font;
    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
}

Label::Label(const std::string &text)
{
    static auto dfont = std::shared_ptr<Font>(new Font("res://calamity/default.ttf"));
    this->font = dfont;

    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->text = text;
    this->setColor({255, 255, 255, 255});
}

Label::~Label()
{
    TTF_DestroyText(handle);
}

Label *Label::setColor(Color _color)
{
    this->color = _color;
    dirty = true;
    TTF_SetTextColor(handle, color.r, color.g, color.b, color.a);
    return this;
}

Label *Label::setDirection(FontDirection _direction)
{
    this->direction = _direction;
    dirty = true;
    TTF_SetTextDirection(handle, static_cast<TTF_Direction>(direction));
    return this;
}

Label *Label::setText(const std::string &_text)
{
    this->text = _text;
    dirty = true;
    TTF_SetTextString(handle, text.c_str(), text.size());
    return this;
}

Label *Label::setWrapWidth(const int width)
{
    this->wrapWidth = width;
    dirty = true;
    TTF_SetTextWrapWidth(handle, wrapWidth);
    return this;
}

int Label::getWrapWidth() const
{
    return wrapWidth;
}

void Label::render(std::shared_ptr<Window> window) {
    Services::graphics()->renderComponent(*this, window.get());
}
