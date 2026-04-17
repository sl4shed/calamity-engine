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

Color::Color(int r, int g, int b) : r(r), g(g), b(b), a(255) {}
Color::Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}

Color::Color(int hexCode) {
    r = (hexCode >> 16) & 0xFF;
    g = (hexCode >> 8)  & 0xFF;
    b =  hexCode        & 0xFF;
    a = 255;
}

Color::Color(int hexCode, int a) : Color(hexCode) {
    this->a = a;
}

Color::Color(std::string hexCode) {
    if (!hexCode.empty() && hexCode[0] == '#')
        hexCode = hexCode.substr(1);
    unsigned int hex = std::stoul(hexCode, nullptr, 16);
    if (hexCode.size() == 8) { // RRGGBBAA
        r = (hex >> 24) & 0xFF;
        g = (hex >> 16) & 0xFF;
        b = (hex >> 8)  & 0xFF;
        a =  hex        & 0xFF;
    } else { // RRGGBB
        r = (hex >> 16) & 0xFF;
        g = (hex >> 8)  & 0xFF;
        b =  hex        & 0xFF;
        a = 255;
    }
}

Color::Color(std::string hexCode, int a) : Color(hexCode) {
    this->a = a;
}

Font::Font(std::string path) {
    this->handle = TTF_OpenFont(path.c_str(), size);
    if(!this->handle) Logger::debug("font load failed: {}", SDL_GetError());

    this->lineSpacing = TTF_GetFontLineSkip((TTF_Font*)handle);
}

TTF_Font* Font::getHandle() {
    return handle;
}

Font::~Font() {
    TTF_CloseFont((TTF_Font*)handle);
}

Font* Font::setKerning(bool enabled) {
    this->kerning = enabled;
    TTF_SetFontKerning((TTF_Font*)handle, enabled);
    return this;
}

Font* Font::setHinting(FontHinting setting) {
    this->hinting = setting;
    TTF_SetFontHinting((TTF_Font*)handle, (TTF_HintingFlags)setting);
    return this;
}

Font* Font::setLanguage(std::string _language) {
    this->language = _language;
    TTF_SetFontLanguage((TTF_Font*)handle, language.c_str());
    return this;
}

Font* Font::setLineSpacing(int _spacing) {
    this->lineSpacing = _spacing;
    TTF_SetFontLineSkip((TTF_Font*)handle, lineSpacing);
    return this;
}

Font* Font::setOutline(int _outline) {
    this->outline = _outline;
    TTF_SetFontOutline((TTF_Font*)handle, outline);
    return this;
}

Font* Font::setSDF(bool enabled) {
    this->SDF = enabled;
    TTF_SetFontSDF((TTF_Font*)handle, enabled);
    return this;
}

Font* Font::setSize(int ptSize) {
    size = ptSize;
    TTF_SetFontSize((TTF_Font*)handle, size);
    return this;
}

Font* Font::setStyle(FontStyle _style) {
    this->style = _style;
    TTF_SetFontStyle((TTF_Font*)handle, (TTF_FontStyleFlags)style);
    return this;
}

Font* Font::setAlignment(FontAlignment _alignment) {
    this->alignment = _alignment;
    TTF_SetFontWrapAlignment((TTF_Font*)handle, (TTF_HorizontalAlignment)alignment);
    return this;
}