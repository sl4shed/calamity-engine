#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../definitions.hpp"

class Color {
public:
    int r; // Red value. Goes up to 255.
    int g; // Green value. Goes up to 255.
    int b; // Blue value. Goes up to 255.
    int a; // Alpha value. Goes up to 255.

    Color(int r, int g, int b);
    Color(int r, int g, int b, int a);
    Color(std::string hexCode);
    Color(std::string hexCode, int a);

    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color TRANSPARENT;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(r, g, b, a);
    }
};

enum class FontHinting {
    INVALID = -1,
    NORMAL,
    LIGHT,
    MONO,
    NONE,
    LIGHT_SUBPIXEL
};

enum class FontStyle {
    NORMAL = 0x00,
    BOLD = 0x01,
    ITALIC = 0x02,
    UNDERLINE = 0x04,
    STRIKETHROUGH = 0x08
};

enum class FontDirection {
    INVALID,
    LTR = 4,
    RTL,
    TTB,
    BTT
};

enum class FontAlignment {
    INVALID = -1,
    LEFT,
    CENTER,
    RIGHT
};

class Font {
public:
    Font();
    Font(std::string path);
    ~Font();
    std::string path; // File path of the font.
    int size = 30;
    
    Font* setKerning(bool enabled);
    Font* setHinting(FontHinting setting);
    Font* setLanguage(std::string language); // bcp47 code please
    Font* setLineSpacing(int spacing);
    Font* setOutline(int outline);
    Font* setSDF(bool enabled);
    Font* setSize(int ptSize);
    Font* setStyle(FontStyle style);
    Font* setAlignment(FontAlignment positioning);

    TTF_Font* getHandle();

    template <class Archive>
    void save(Archive &ar) const {
        ar(path, size);
    }

    template <class Archive>
    void load(Archive &ar) {
        ar(path, size);
        TTF_OpenFont(path.c_str(), size);
    }
private:
    TTF_Font* handle; // again for interchangeability of backends if for whatever reason i wanted to switch from SDL
    // that will always be the sdl3 font thing
};