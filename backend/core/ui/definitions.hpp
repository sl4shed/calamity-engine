#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../definitions.hpp"

/**
 * # Color
 * A simple class that defines a color using red, green, blue and alpha channels.
 *
 * You can construct it with a string hex code, an actual hex code, or provide individual channel values up to 255.
 */
class Color {
public:
    int r; // Red value. Goes up to 255.
    int g; // Green value. Goes up to 255.
    int b; // Blue value. Goes up to 255.
    int a; // Alpha value. Goes up to 255.

    Color(int r, int g, int b);
    Color(int r, int g, int b, int a);
    Color(int hexCode);
    Color(int hexCode, int a);
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

/**
 * Mirrors the [TTF_HintingFlags](https://wiki.libsdl.org/SDL3_ttf/TTF_HintingFlags) enum.
 * Dictates font hinting.
 */
enum class FontHinting {
    INVALID = -1,
    NORMAL,
    LIGHT,
    MONO,
    NONE,
    LIGHT_SUBPIXEL
};

/**
 * Mirrors the [TTF_FontStyleFlags](https://wiki.libsdl.org/SDL3_ttf/TTF_FontStyleFlags) enum.
 * Dictates the font style used (bold, italic, underline, etc...).
 */
enum class FontStyle {
    NORMAL = 0x00,
    BOLD = 0x01,
    ITALIC = 0x02,
    UNDERLINE = 0x04,
    STRIKETHROUGH = 0x08
};

/**
 * Mirrors the [TTF_Direction](https://wiki.libsdl.org/SDL3_ttf/TTF_Direction) enum.
 * Dictates the direction of the text (left to right, right to left, etc...).
 */
enum class FontDirection {
    INVALID,
    LTR = 4,
    RTL,
    TTB,
    BTT
};

/**
 * Mirrors the [TTF_HorizontalAlignment](https://wiki.libsdl.org/SDL3_ttf/TTF_HorizontalAlignment) enum.
 * Dictates the horizontal alignment of the font.
 */
enum class FontAlignment {
    INVALID = -1,
    LEFT,
    CENTER,
    RIGHT
};

/**
 * # Font
 * Allows the usage of TrueType fonts. Has plenty of settings aswell!
 *
 * Example usage:
 * ```cpp
 * Font font = Font("path/to/font");
 * font.setKerning(true)->setLineSpacing(10)->setSize(30)->setStyle(FontStyle::UNDERLINE);
 * ```
 */
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
    TTF_Font* handle;
};