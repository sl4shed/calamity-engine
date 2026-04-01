#include "definitions.hpp"
#include "../../utils/logger.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_ttf/SDL_textengine.h>

const Color Color::WHITE = Color(255, 255, 255, 255);
const Color Color::BLACK = Color(0, 0, 0, 255);
const Color Color::RED = Color(255, 0, 0, 255);
const Color Color::GREEN = Color(0, 255, 0, 255);
const Color Color::BLUE = Color(0, 0, 255, 255);
const Color Color::TRANSPARENT = Color(0, 0, 0, 0);

Color::Color(int r, int g, int b) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
}

Color::Color(int r, int g, int b, int a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

// todo
Color::Color(std::string hexCode) {}
Color::Color(std::string hexCode, int a) {}

Font::Font(std::string path) {
    this->handle = TTF_OpenFont(path.c_str(), size);
    if(!this->handle) Logger::debug("font load failed: {}", SDL_GetError());
}

TTF_Font* Font::getHandle() {
    return handle;
}

Font::~Font() {
    TTF_CloseFont((TTF_Font*)handle);
}

Font* Font::setKerning(bool enabled) {
    TTF_SetFontKerning((TTF_Font*)handle, enabled);
    return this;
}

Font* Font::setHinting(FontHinting setting) {
    TTF_SetFontHinting((TTF_Font*)handle, (TTF_HintingFlags)setting);
    return this;
}

Font* Font::setLanguage(std::string language) {
    TTF_SetFontLanguage((TTF_Font*)handle, language.c_str());
    return this;
}

Font* Font::setLineSpacing(int spacing) {
    TTF_SetFontLineSkip((TTF_Font*)handle, spacing);
    return this;
}

Font* Font::setOutline(int outline) {
    TTF_SetFontOutline((TTF_Font*)handle, outline);
    return this;
}

Font* Font::setSDF(bool enabled) {
    TTF_SetFontSDF((TTF_Font*)handle, enabled);
    return this;
}

Font* Font::setSize(int ptSize) {
    size = ptSize;
    TTF_SetFontSize((TTF_Font*)handle, size);
    return this;
}

Font* Font::setStyle(FontStyle style) {
    TTF_SetFontStyle((TTF_Font*)handle, (TTF_FontStyleFlags)style);
    return this;
}

Font* Font::setAlignment(FontAlignment alignment) {
    TTF_SetFontWrapAlignment((TTF_Font*)handle, (TTF_HorizontalAlignment)alignment);
    return this;
}