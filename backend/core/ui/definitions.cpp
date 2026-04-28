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

Color::Color(const int r, const int g, const int b) : r(r), g(g), b(b), a(255) {}
Color::Color(const int r, const int g, const int b, const int a) : r(r), g(g), b(b), a(a) {}

Color::Color(const int hexCode) {
    r = (hexCode >> 16) & 0xFF;
    g = (hexCode >> 8)  & 0xFF;
    b =  hexCode        & 0xFF;
    a = 255;
}

Color::Color(const int hexCode, const int a) : Color(hexCode) {
    this->a = a;
}

Color::Color(std::string hexCode) {
    // black magic
    if (!hexCode.empty() && hexCode[0] == '#')
        hexCode = hexCode.substr(1);
    const unsigned int hex = std::stoul(hexCode, nullptr, 16);
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

Color::Color(const std::string& hexCode, const int a) : Color(hexCode) {
    this->a = a;
}

Font::Font(const std::string& path) {
    this->handle = TTF_OpenFont(path.c_str(), static_cast<float>(size));
    if(!this->handle) Logger::debug("Loading font {} failed: {}", path, SDL_GetError());

    this->lineSpacing = TTF_GetFontLineSkip(handle);
}

TTF_Font* Font::getHandle() const
{
    return handle;
}

Font::~Font() {
    TTF_CloseFont(handle);
}

Font* Font::setKerning(const bool enabled) {
    this->kerning = enabled;
    TTF_SetFontKerning(handle, enabled);
    return this;
}

Font* Font::setHinting(FontHinting setting) {
    this->hinting = setting;
    TTF_SetFontHinting(handle, static_cast<TTF_HintingFlags>(setting));
    return this;
}

Font* Font::setLanguage(const std::string& _language) {
    this->language = _language;
    TTF_SetFontLanguage(handle, language.c_str());
    return this;
}

Font* Font::setLineSpacing(const int _spacing) {
    this->lineSpacing = _spacing;
    TTF_SetFontLineSkip(handle, lineSpacing);
    return this;
}

Font* Font::setOutline(const int _outline) {
    this->outline = _outline;
    TTF_SetFontOutline(handle, outline);
    return this;
}

Font* Font::setSDF(const bool enabled) {
    this->SDF = enabled;
    TTF_SetFontSDF(handle, enabled);
    return this;
}

Font* Font::setSize(const int ptSize) {
    size = ptSize;
    TTF_SetFontSize(handle, static_cast<float>(ptSize));
    return this;
}

Font* Font::setStyle(const FontStyle _style) {
    this->style = _style;
    TTF_SetFontStyle(handle, static_cast<TTF_FontStyleFlags>(style));
    return this;
}

Font* Font::setAlignment(FontAlignment _alignment) {
    this->alignment = _alignment;
    TTF_SetFontWrapAlignment(handle, static_cast<TTF_HorizontalAlignment>(alignment));
    return this;
}