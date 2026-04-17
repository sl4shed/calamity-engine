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
}

std::string Label::getText() const {
    return std::string(handle->text);
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
}

Label::Label(std::string text) {
#ifdef EMSCRIPTEN
    static Font dfont = Font("/calamity/default.ttf");
#else
    static Font dfont = Font("./calamity/default.ttf");
#endif
    this->font = &dfont;
    this->handle = TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
    this->setColor({255, 255, 255, 255});
}

Label::~Label() {
    TTF_DestroyText((TTF_Text*)handle);
}

Label* Label::setColor(Color _color) {
    this->color = _color;
    TTF_SetTextColor(handle, color.r, color.g, color.b, color.a);
    return this;
}

Label* Label::setDirection(FontDirection _direction) {
    this->direction = _direction;
    TTF_SetTextDirection(handle, (TTF_Direction)direction);
    return this;
}

Label* Label::setText(std::string _text) {
    this->text = _text;
    TTF_SetTextString(handle, text.c_str(), text.size());
    return this;
}

Label* Label::setWrapWidth(int width) {
    this->wrapWidth = width;
    TTF_SetTextWrapWidth(handle, wrapWidth);
    return this;
}

int Label::getWrapWidth() {
    return wrapWidth;
}